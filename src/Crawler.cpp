#include <cstdlib>
#include <ctime>

#include <fstream>
#include <set>
#include <string>
#include <queue>
#include <iostream>

#include "Crawler.h"
#include "Downloader.h"

std::string Crawler::GenerateFileName()
{
  return data_directory_ +
    std::to_string(time(NULL)) + " " + std::to_string(rand());
}

bool Crawler::FetchNextPage()
{
  if (download_queue_.empty())
  {
    return false;
  }
  std::string link = download_queue_.front();
  if (link.compare(0, root_path_.length(), root_path_) != 0)
  {
    link = root_path_ + link;
  }
  std::string filename = GenerateFileName();
  int result = Downloader::DownloadPage(link, filename);
  download_queue_.pop();

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
    } while (ch != '"');
    if (!flag ||
        link.compare(0,
          required_link_prefix_.length(),
          required_link_prefix_) != 0)
    {
      continue;
    }
    if (visited_links_.count(link) == 0)
    {
      visited_links_.insert(link);
      download_queue_.push(link);
    }
  }
}

Crawler::Crawler(const std::string& data_directory,
    const std::string& root_path,
    const std::string& required_link_prefix) :
  data_directory_(data_directory),
  root_path_(root_path),
  required_link_prefix_(required_link_prefix)
{ }

void Crawler::Crawl(const std::string& start_page)
{
  download_queue_.push(start_page);
  visited_links_.insert(start_page);
  while (FetchNextPage())
  {
  }
}
