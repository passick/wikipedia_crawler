#ifndef INDEXER_H
#define INDEXER_H

#include <csignal>

#include <string>
#include <unordered_set>
#include <unordered_map>
#include <ostream>
#include <vector>

#include "SaveableStringContainer.h"

class FilenameAndLink
{
  public:
    std::string filename;
    std::string link;

    FilenameAndLink();
    explicit FilenameAndLink(const std::string& string);
    FilenameAndLink(const std::string& filename, const std::string& link);

    bool operator<(const FilenameAndLink& rhs) const;
    bool operator==(const FilenameAndLink& rhs) const;

    friend std::ostream& operator<<(std::ostream& stream,
        const FilenameAndLink& object);
    friend std::istream& operator>>(std::istream& stream,
        const FilenameAndLink& object);
};

namespace std {
  template <>
  struct hash<FilenameAndLink>
  {
    size_t operator()(const FilenameAndLink& obj) const;
  };
}

class Indexer
{
 private:
  static const int not_saved_indexes_limit_;
  std::string files_directory_;
  std::string index_directory_;
  std::vector<std::string> files_in_directory_;
  std::unordered_map<std::string,
    std::vector<FilenameAndLink>> not_saved_indexes_;
  SaveableStringContainer<
    std::unordered_set<FilenameAndLink>> indexed_files_;
  volatile static sig_atomic_t indexer_terminated;

  void SaveWordIndex(
      const std::string& word,
      bool delete_from_not_saved = true);

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
