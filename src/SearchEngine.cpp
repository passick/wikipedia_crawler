#include <fstream>
#include <vector>
#include <algorithm>

#include "SearchEngine.h"

SearchEngine::SearchEngine(const std::string& index_directory,
    const std::string& pagerank_filename) :
  index_directory_(index_directory)
{ 
  std::ifstream pagerank_file(pagerank_filename);
  std::string file_line;
  while (std::getline(pagerank_file, file_line, '\t'))
  {
    FilenameAndLink entry(file_line);
    long double rank;
    pagerank_file >> rank;

    pagerank_.insert({entry, rank});
  }
}

std::vector<FilenameAndLink> SearchEngine::Find(const std::string& query)
{
  std::string modified_query;
  for (char ch : query)
  {
    modified_query.append(1, tolower(ch));
  }
  std::ifstream index_file(index_directory_ + modified_query.substr(0, 3));
  if (!index_file.good())
  {
    return std::vector<FilenameAndLink>();
  }
  std::vector<FilenameAndLink> result;
  std::string line;
  while (std::getline(index_file, line))
  {
    while (line.back() != ':' && std::getline(index_file, line))
    { }
    if (!index_file.good())
    {
      break;
    }
    bool query_matches = (line.substr(0, line.length() - 1) == modified_query);
    int previous_position = index_file.tellg();
    std::getline(index_file, line);
    while (line.back() != ':')
    {
      if (query_matches)
      {
        result.push_back(FilenameAndLink(line));
      }
      previous_position = index_file.tellg();
      if (!std::getline(index_file, line))
      {
        break;
      }
    }
    if (index_file.good())
    {
      index_file.seekg(previous_position, std::ios_base::beg);
    }
  }
  std::sort(result.begin(), result.end(),
      [&](const FilenameAndLink& entry1, const FilenameAndLink& entry2)
      {
        return pagerank_[entry1] < pagerank_[entry2];
      });
  for (int i = 0; i < result.size() - 1; ++i)
  {
    if (pagerank_[result[i]] <= pagerank_[result[i + 1]])
    {
      std::cout << i << std::endl;
    }
  }
  return result;
}
