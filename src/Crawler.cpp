#include <csignal>
#include <cstdlib>
#include <ctime>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <set>
#include <string>
#include <iostream>
#include <deque>
#include <vector>

#include "Crawler.h"
#include "Downloader.h"
#include "HTMLContent.h"
#include "HTMLTag.h"
#include "WikiPage.h"

volatile sig_atomic_t Crawler::program_terminated = false;

std::string Crawler::GenerateFileName()
{
  // FIXME: rand() is not thread-safe.
  return data_directory_ +
    std::to_string(time(NULL)) + " " + std::to_string(rand());
}

// Return value:
// 0 - success
// 1 - failed to download
// 2 - received interruption signal
// 3 - download queue is empty
int Crawler::FetchNextPage()
{
  pthread_mutex_lock(&queue_and_visited_mutex_);
  if (download_queue_.container.empty())
  {
    pthread_mutex_unlock(&queue_and_visited_mutex_);
    sleep(wait_for_queue_refill_);
    pthread_mutex_lock(&queue_and_visited_mutex_);

    if (download_queue_.container.empty())
    {
      pthread_mutex_unlock(&queue_and_visited_mutex_);
      return 3;
    }
  }
  std::string link = download_queue_.container.front();
  download_queue_.container.pop_front();
  pthread_mutex_unlock(&queue_and_visited_mutex_);

  std::string original_link = link;
  if (link.compare(0, root_path_.length(), root_path_) != 0)
  {
    link = root_path_ + link;
  }
  std::string filename = GenerateFileName();
  std::cout << "Downloading " << link << std::endl;
  int result = Downloader::DownloadPage(link, filename);
  // check if program got signal to quit
  if (WIFSIGNALED(result) &&
      (WTERMSIG(result) == SIGINT || WTERMSIG(result) == SIGQUIT ||
       WTERMSIG(result) == SIGTERM))
  {
    SaveState();
    program_terminated = true;
    return 2;
  }
  // decode value that was returned by system() call
  result = WEXITSTATUS(result);
  if (result != 0)
  {
    std::cerr << "\033[1;31mFailed to fetch page " <<
      link << "\033[0m" << std::endl;
    if (result != 8)
    {
      pthread_mutex_lock(&queue_and_visited_mutex_);
      download_queue_.container.push_front(original_link);
      pthread_mutex_unlock(&queue_and_visited_mutex_);
    }
    return 1;
  }

  ParsePage(original_link, filename);
  return 0;
}

void Crawler::ParsePage(const std::string& link, const std::string& filename)
{
  std::ifstream file(filename);
  std::string html, temp;
  while (std::getline(file, temp))
  {
    html += temp + '\n';
  }
  WikiPage page = WikiPage(*HTMLContent(html, 0).tags()[1]);
  std::ofstream article_text_file(filename + ".text");
  article_text_file << link << std::endl << page.text();
  std::ofstream found_links_file(filename + ".links");
  found_links_file << link << std::endl << std::endl;
  pthread_mutex_lock(&queue_and_visited_mutex_);
  for (const std::string& found_link : page.links())
  {
    found_links_file << found_link << std::endl;
    AddLink(found_link);
  }
  pthread_mutex_unlock(&queue_and_visited_mutex_);
  if (article_text_file.good() && found_links_file.good())
  {
    file.close();
    std::remove(filename.c_str());
  }
}

bool Crawler::AddLink(const std::string& link)
{
  if (visited_links_.container.count(link) != 0)
  {
    return false;
  }
  visited_links_.container.insert(link);
  download_queue_.container.push_back(link);

  return true;
}

void Crawler::SaveState()
{
  pthread_mutex_lock(&queue_and_visited_mutex_);
  visited_links_.Save();
  download_queue_.Save();
  pthread_mutex_unlock(&queue_and_visited_mutex_);
}

void Crawler::StopCrawler(int signal)
{
  program_terminated = true;
}

Crawler::Crawler(const std::string& data_directory,
    const std::string& root_path,
    const std::string& start_page) :
  data_directory_(data_directory),
  root_path_(root_path)
{
  download_queue_ =
    SaveableStringContainer<std::deque<std::string>>
    (data_directory_ + queue_filename_);
  visited_links_ =
    SaveableStringContainer<std::unordered_set<std::string>>
    (data_directory_ + visited_filename_);

  if (!start_page.empty())
  {
    AddLink(start_page);
  }
}

void Crawler::Crawl()
{
  std::signal(SIGINT, StopCrawler);
  std::signal(SIGTERM, StopCrawler);
  std::signal(SIGQUIT, StopCrawler);

  while (!program_terminated && FetchNextPage() <= 1)
  { }

  std::signal(SIGINT, SIG_DFL);
  std::signal(SIGTERM, SIG_DFL);
  std::signal(SIGQUIT, SIG_DFL);
}

void *Crawler::MultithreadCrawl(void *crawler_ptr)
{
  Crawler *crawler = static_cast<Crawler*>(crawler_ptr);
  crawler->Crawl();

  return NULL;
}
