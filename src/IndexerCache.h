#ifndef INDEXERCACHE_H
#define INDEXERCACHE_H

#include <pthread.h>

#include <unordered_map>
#include <vector>

#include "FilenameAndLink.h"

const int changes_number_threshold = 100000;

class IndexerCache
{
  std::unordered_map<std::string,
    std::vector<FilenameAndLink>> cache_;
  std::string index_directory_;
  pthread_mutex_t cache_mutex_ = PTHREAD_MUTEX_INITIALIZER;
  int changes_count_ = 0;
  void SaveFiles();

 public:
  IndexerCache(const std::string& index_directory);
  ~IndexerCache();
  void AddToFile(const std::string& filename, const FilenameAndLink& entry);
};

#endif
