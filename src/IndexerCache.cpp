#include <pthread.h>

#include <fstream>
#include <iostream>

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
  cache_[filename].push_back(entry);
  changes_count_++;
  pthread_mutex_unlock(&cache_mutex_);
  if (changes_count_ > changes_number_threshold)
  {
    SaveFiles();
  }
}

void IndexerCache::SaveFiles()
{
  std::cout << "Saving index files" << std::endl;
  pthread_mutex_lock(&cache_mutex_);
  
  std::unordered_map<std::string, std::ofstream> open_files;
  
  for (const auto& file_and_list : cache_)
  {
    std::string filename = file_and_list.first.substr(0, 3);
    if (open_files.count(filename) == 0)
    {
      open_files[file_and_list.first].open(index_directory_ + filename,
          std::ofstream::out | std::ofstream::app);
      if (!open_files[file_and_list.first].good())
      {
        continue;
      }
    }
    open_files[filename] << file_and_list.first << ":\n";
    for (const auto& entry : file_and_list.second)
    {
      open_files[filename] << entry << '\n';
    }
  }

  cache_.clear();
  changes_count_ = 0;
  pthread_mutex_unlock(&cache_mutex_);
}
