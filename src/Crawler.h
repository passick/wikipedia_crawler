#ifndef CRAWLER_H
#define CRAWLER_H

#include <csignal>

#include <unordered_set>
#include <string>
#include <deque>
#include <vector>
#include <pthread.h>

#include "SaveableStringContainer.h"

class Crawler
{
  SaveableStringContainer<std::unordered_set<std::string>> visited_links_;
  SaveableStringContainer<std::deque<std::string>> download_queue_;

  std::string data_directory_;
  std::string root_path_;
  std::string download_queue_file_;

  const std::string link_prefix_ = "<a href=\"";
  const std::string queue_filename_ = "queue.tmp";
  const std::string visited_filename_ = "visited.tmp";
  const int wait_for_queue_refill_ = 10;

  volatile static sig_atomic_t program_terminated;
  pthread_mutex_t queue_and_visited_mutex_ = PTHREAD_MUTEX_INITIALIZER;

  std::string GenerateFileName();
  int FetchNextPage();
  void ParsePage(const std::string& link, const std::string& filename);
  bool AddLink(const std::string& link);
  void SaveState();

  static void StopCrawler(int signal);

 public:
  Crawler(const std::string& data_directory,
      const std::string& root_path,
      const std::string& start_page = "");

  void Crawl();
  static void *MultithreadCrawl(void *crawler);
};

#endif
