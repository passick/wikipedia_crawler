#include <csignal>
#include <cmath>
#include <dirent.h>

#include <string>
#include <fstream>
#include <iostream>
#include <unordered_set>
#include <algorithm>

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
  probabilities_.assign(index_to_filename_.size(),
      1.0L / (long double)index_to_filename_.size());
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
  std::cout << "PageRanker started scanning." << std::endl;
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
  std::cout << "Building graph." << std::endl;
  int count = 0;
  for (const auto& pair : filename_to_index_)
  {
    if (pageranker_terminated)
    {
      break;
    }
    std::string filename = pair.first;
    std::size_t index = pair.second;
    graph_[index].push_back(index);
    std::string article_name;
    std::ifstream file(filename);
    std::getline(file, article_name);
    
    std::string link;
    std::getline(file, link); // read empty line
    while (std::getline(file, link))
    {
      if (article_name_to_index_.count(link) == 0)
      {
        continue;
      }
      graph_[index].push_back(article_name_to_index_[link]);
    }
    count++;
    if (count % messages_threshold == 0)
    {
      std::cout << "Graph currently has " <<
        count << " vertices." << std::endl;
    }
  }
  std::cout << "Graph created." << std::endl;
}

void PageRanker::StopPageRanker(int signal)
{
  pageranker_terminated = signal;
}

long double PageRanker::PerformProbabilitiesIteration()
{
  std::vector<long double> new_probabilities(graph_.size(), 0);
  for (std::size_t from = 0; from < graph_.size(); ++from)
  {
    for (std::size_t to : graph_[from])
    {
      new_probabilities[to] += probabilities_[from] /
        (long double)graph_[from].size();
    }
  }
  long double delta = 0;
  for (std::size_t page = 0; page < graph_.size(); ++page)
  {
    long double new_probability = random_page_probability +
      (1 - random_page_probability) * new_probabilities[page];
    //long double new_probability = new_probabilities[page];
    delta = std::max(delta, std::fabs(new_probability - probabilities_[page]));
    probabilities_[page] = new_probability;
  }
  return delta;
}

void PageRanker::FindProbabilities(long double precision)
{
  long double delta = 1;
  while (delta > precision)
  {
    delta = PerformProbabilitiesIteration();
    std::cout << "New pagerank delta = " << delta << std::endl;
  }
}

void PageRanker::SaveProbabilities(const std::string& filename)
{
  std::vector<std::pair<long double, FilenameAndLink>> sorted_articles;
  for (const auto& entry : articles_map_.container)
  {
    if (filename_to_index_.count(entry.filename) == 0)
    {
      throw std::runtime_error("Could not find " + entry.filename);
    }
    sorted_articles.push_back({
        probabilities_[filename_to_index_[entry.filename]],
        entry});
  }
  std::sort(sorted_articles.rbegin(), sorted_articles.rend());
  std::ofstream file(filename);
  for (const auto& pair : sorted_articles)
  {
    file << pair.second << '\t' << pair.first << std::endl;
  }
}
