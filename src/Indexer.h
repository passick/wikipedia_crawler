#ifndef INDEXER_H
#define INDEXER_H

#include <string>
#include <unordered_set>
#include <ostream>

#include "SaveableStringContainer.h"

class FilenameAndLink
{
  public:
    std::string filename;
    std::string link;

    FilenameAndLink();
    FilenameAndLink(const std::string& string);

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
 public:
 private:
  std::string files_directory_;
  std::string index_directory_;
  SaveableStringContainer<
    std::unordered_set<FilenameAndLink>> indexed_files_;

 public:
  Indexer(const std::string& files_directory,
      const std::string& index_directory,
      const std::string& list_of_indexed_files);
};

#endif
