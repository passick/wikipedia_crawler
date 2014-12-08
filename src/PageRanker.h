#ifndef PAGERANKER_H
#define PAGERANKER_H

#include <csignal>

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include "SaveableStringContainer.h"
#include "FilenameAndLink.h"

class PageRanker
{
  std::vector<long double> probabilities_;

  std::vector<std::string> index_to_filename_;
  std::vector<std::string> index_to_article_name_;
  std::unordered_map<std::string, std::size_t> filename_to_index_;
  std::unordered_map<std::string, std::size_t> article_name_to_index_;

  std::vector<std::vector<std::size_t>> graph_;

  volatile static sig_atomic_t pageranker_terminated;

  const long double random_page_probability = 1e-12L;

  SaveableStringContainer<std::unordered_set<FilenameAndLink>> articles_map_;
  
  void ScanDirectory(const std::string& pages_directory);
  void BuildGraph();
  static void StopPageRanker(int signal);
  long double PerformProbabilitiesIteration();

 public:
  PageRanker(
      const std::string& pages_directory,
      const std::string& articles_map_file);
  void FindProbabilities(long double precision = 5e-7L);
  void SaveProbabilities(const std::string& filename);
};

#endif
