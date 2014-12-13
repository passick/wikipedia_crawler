#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>

#include "FilenameAndLink.h"

class SearchEngine
{
  std::string index_directory_;
  std::unordered_map<FilenameAndLink, long double> pagerank_;

 public:
  SearchEngine(const std::string& index_directory,
      const std::string& pagerank_filename);
  std::vector<FilenameAndLink> Find(const std::string&);
};

#endif
