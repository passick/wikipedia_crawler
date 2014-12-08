#include <pthread.h>

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <map>
#include <set>

#include "IndexerCache.h"
#include "FilenameAndLink.h"

IndexerCache::IndexerCache(const std::string& index_directory) :
  index_directory_(index_directory)
{ }

IndexerCache::~IndexerCache()
{
  SaveFiles();
}

void IndexerCache::AddToFile(const std::string& filename, const FilenameAndLink& entry)
{
  pthread_mutex_lock(&cache_mutex_);
  cache_[filename].insert(entry);
  changes_count_++;

  if (changes_count_ > changes_number_threshold)
  {
    changes_count_ = 0;
    pthread_mutex_unlock(&cache_mutex_);
    SaveFiles();
  }
  else
  {
    pthread_mutex_unlock(&cache_mutex_);
  }
}

void IndexerCache::SaveFiles()
{
  std::cout << "Saving index files" << std::endl;
  pthread_mutex_lock(&cache_mutex_);
  
  std::map<std::string, std::set<FilenameAndLink>> sorted_cache(
      cache_.begin(), cache_.end());
  std::string previous_filename = "";
  std::ofstream file;
  for (const auto& file_and_list : sorted_cache)
  {
    std::string filename = file_and_list.first.substr(0, 3);
    if (filename != previous_filename)
    {
      if (file.is_open())
      {
        file.close();
      }
      file.open(index_directory_ + filename,
          std::ofstream::out | std::ofstream::app);
      if (!file.good())
      {
        throw std::runtime_error(
            "Could not open file" + index_directory_ + filename);
      }
    }
    file << file_and_list.first << ":\n";
    for (const auto& entry : file_and_list.second)
    {
      file << entry << '\n';
    }
  }

  cache_.clear();
  changes_count_ = 0;
  pthread_mutex_unlock(&cache_mutex_);
}
