#include <csignal>
#include <dirent.h>

#include <string>
#include <fstream>
#include <iostream>

#include "PageRanker.h"
#include "FilenameAndLink.h"

volatile sig_atomic_t PageRanker::pageranker_terminated = 0;
const std::size_t messages_threshold = 2000;

PageRanker::PageRanker(
    const std::string& pages_directory,
    const std::string& articles_map_file)
{
  articles_map_ =
    SaveableStringContainer<
    std::unordered_set<FilenameAndLink>>(articles_map_file);
  std::signal(SIGINT, StopPageRanker);
  std::signal(SIGTERM, StopPageRanker);
  std::signal(SIGQUIT, StopPageRanker);

  ScanDirectory(pages_directory);
  BuildGraph();

  std::signal(SIGINT, SIG_DFL);
  std::signal(SIGTERM, SIG_DFL);
  std::signal(SIGQUIT, SIG_DFL);

  if (pageranker_terminated)
  {
    articles_map_.Save();
    raise(pageranker_terminated);
  }
}

void PageRanker::ScanDirectory(const std::string& pages_directory)
{
  for (const FilenameAndLink& entry : articles_map_.container)
  {
    filename_to_index_[entry.filename] = index_to_filename_.size();
    article_name_to_index_[entry.link] = index_to_filename_.size();
    index_to_filename_.push_back(entry.filename);
    index_to_article_name_.push_back(entry.link);
  }
  std::cout << "Pagerank started scanning" << std::endl;
  struct dirent *entry;
  DIR *directory;
  directory = opendir(pages_directory.c_str());
  if (directory == NULL)
  {
    perror("opendir");
    return;
  }
  while ((entry = readdir(directory)) && !pageranker_terminated)
  {
    // FIXME: add a check for file not being a directory
    std::string filename(entry->d_name);
    filename = pages_directory + filename;
    const std::string links_file_ending = ".links";
    if (filename.compare(
          filename.length() >= links_file_ending.length() ?
          filename.length() - links_file_ending.length() :
          0,
          links_file_ending.length(),
          links_file_ending))
    {
      continue;
    }
    if (articles_map_.container.count(FilenameAndLink(filename, "")) > 0)
    {
      continue;
    }
    std::ifstream file(filename);
    std::string article_name;
    std::getline(file, article_name);

    if (index_to_filename_.size() != 0 &&
        index_to_filename_.size() % messages_threshold == 0)
    {
      std::cout << "Scanned " <<
        index_to_filename_.size() <<
        " articles" << std::endl;
    }
    articles_map_.container.insert(
        FilenameAndLink(filename, article_name));
    filename_to_index_[filename] = index_to_filename_.size();
    article_name_to_index_[article_name] = index_to_filename_.size();
    index_to_filename_.push_back(filename);
    index_to_article_name_.push_back(article_name);
  }
  probabilities_.assign(index_to_filename_.size(), 0);
  graph_ = std::vector<std::vector<std::size_t>>(index_to_filename_.size());
}

void PageRanker::BuildGraph()
{
  for (const auto& pair : filename_to_index_)
  {
    if (pageranker_terminated)
    {
      break;
    }
    std::string filename = pair.first;
    std::size_t index = pair.second;
    std::string article_name;
    std::ifstream file(filename);
    std::getline(file, article_name);
    
    std::string link;
    std::getline(file, link); // empty line
    while (std::getline(file, link))
    {
      if (article_name_to_index_.count(link) == 0)
      {
        continue;
      }
      graph_[index].push_back(article_name_to_index_[link]);
    }
    std::cout << article_name << ' ' << graph_[index].size() << std::endl;
  }
}

void PageRanker::StopPageRanker(int signal)
{
  pageranker_terminated = signal;
}
