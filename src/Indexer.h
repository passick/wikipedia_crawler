#ifndef INDEXER_H
#define INDEXER_H

#include <csignal>

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <ostream>
#include <vector>

#include "SaveableStringContainer.h"
#include "FilenameAndLink.h"
#include "IndexerCache.h"

class Indexer
{
 private:
  std::string files_directory_;
  std::string index_directory_;
  std::vector<std::string> files_in_directory_;
  IndexerCache file_cache_;
  SaveableStringContainer<
    std::unordered_set<FilenameAndLink>> indexed_files_;
  volatile static sig_atomic_t indexer_terminated;

 public:
  Indexer(const std::string& files_directory,
      const std::string& index_directory,
      const std::string& list_of_indexed_files);

  void GetFileList();
  static void StopIndexer(int signal);
  void StartIndexing();
  void AddWordOccurrence(
      const std::string& word,
      const std::string& filename,
      const std::string& link);
  void IndexFile(const std::string& filename);
};

#endif
