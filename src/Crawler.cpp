#include <cstdlib>
#include <ctime>
#include <sys/wait.h>

#include <fstream>
#include <set>
#include <string>
#include <iostream>
#include <deque>
#include <vector>

#include "Crawler.h"
#include "Downloader.h"

std::string Crawler::GenerateFileName()
{
  return data_directory_ +
    std::to_string(time(NULL)) + " " + std::to_string(rand());
}

bool Crawler::FetchNextPage()
{
  if (download_queue_.container.empty())
  {
    return false;
  }
  std::string link = download_queue_.container.front();
  if (link.compare(0, root_path_.length(), root_path_) != 0)
  {
    link = root_path_ + link;
  }
  std::string filename = GenerateFileName();
  std::cout << "Downloading " << link << std::endl;
  int result = Downloader::DownloadPage(link, filename);
  // check if program got signal to quit
  if (WIFSIGNALED(result) &&
      (WTERMSIG(result) == SIGINT || WTERMSIG(result) == SIGQUIT))
  {
    return false;
  }
  // decode value that was returned by system() call
  result = WEXITSTATUS(result);
  if (result != 0)
  {
    std::cerr << "\033[1;31mFailed to fetch page " <<
      link << "\033[0m" << std::endl;
  }
  if (result != 0 && result != 8)
    // 8 means server issued an error response (like 404) and we should
    // probably just ignore it
  {
    return false;
  }
  download_queue_.container.pop_front();

  GetLinks(filename);
  return true;
}

void Crawler::GetLinks(const std::string& filename)
{
  std::ifstream file(filename);
  char ch;
  while (file.get(ch))
  {
    int current = 0;
    while (current < link_prefix_.length() &&
        ch == link_prefix_[current])
    {
      current++;
      if (!file.get(ch))
      {
        break;
      }
    }
    if (current != link_prefix_.length())
    {
      continue;
    }
    std::string link;
    bool flag = true;
    do
    {
      if (ch != '"')
      {
        link.append(1, ch);
      }
      if (!file.get(ch))
      {
        flag = false;
        break;
      }
      for (char banned_symbol : banned_symbols_)
      {
        if (ch == banned_symbol)
        {
          flag = false;
        }
      }
    } while (ch != '"');
    if (!flag ||
        link.compare(0,
          required_link_prefix_.length(),
          required_link_prefix_) != 0)
    {
      continue;
    }
    if (visited_links_.container.count(link) == 0)
    {
      visited_links_.container.insert(link);
      download_queue_.container.push_back(link);
    }
  }
}

Crawler::Crawler(const std::string& data_directory,
    const std::string& root_path,
    const std::string& required_link_prefix,
    const std::vector<char>& banned_symbols) :
  data_directory_(data_directory),
  root_path_(root_path),
  required_link_prefix_(required_link_prefix),
  banned_symbols_(banned_symbols)
{
  download_queue_ =
    SaveableStringContainer<std::deque<std::string>>
    (data_directory_ + queue_filename_);
  visited_links_ =
    SaveableStringContainer<std::unordered_set<std::string>>
    (data_directory_ + visited_filename_);
}

void Crawler::Crawl(const std::string& start_page)
{
  if (download_queue_.container.empty())
  {
    download_queue_.container.push_back(start_page);
    visited_links_.container.insert(start_page);
  }
  while (FetchNextPage())
  {
  }
}
