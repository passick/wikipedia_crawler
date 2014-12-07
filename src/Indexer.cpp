#include <cstring>
#include <dirent.h>

#include <string>
#include <fstream>
#include <algorithm>

#include "Indexer.h"
#include "IndexerCache.h"
#include "FilenameAndLink.h"

volatile sig_atomic_t Indexer::indexer_terminated = false;

Indexer::Indexer(const std::string& files_directory,
      const std::string& index_directory,
      const std::string& list_of_indexed_files) :
  files_directory_(files_directory),
  index_directory_(index_directory),
  file_cache_(index_directory)
{
  indexed_files_ = SaveableStringContainer<
    std::unordered_set<FilenameAndLink>>(list_of_indexed_files);
}

void Indexer::GetFileList()
{
  struct dirent *entry;
  DIR *directory;
  directory = opendir(files_directory_.c_str());
  if (directory == NULL)
  {
    perror("opendir");
    return;
  }
  while ((entry = readdir(directory)))
  {
    std::string filename(entry->d_name);
    filename = files_directory_ + filename;
    const std::string text_file_ending = ".text";
    if (filename.compare(
          filename.length() >= text_file_ending.length() ?
          filename.length() - text_file_ending.length() :
          0,
          text_file_ending.length(),
          text_file_ending))
    {
      continue;
    }
    
    if (indexed_files_.container.count({filename, ""}) > 0)
    {
      continue;
    }
    files_in_directory_.push_back(filename);
  }
  closedir(directory);
}

void Indexer::StopIndexer(int signal)
{
  indexer_terminated = true;
}

void Indexer::StartIndexing()
{
  GetFileList();

  std::signal(SIGINT, StopIndexer);
  std::signal(SIGTERM, StopIndexer);
  std::signal(SIGQUIT, StopIndexer);

  for (const std::string& filename : files_in_directory_)
  {
    if (indexer_terminated)
    {
      break;
    }
    IndexFile(filename);
  }

  std::signal(SIGINT, SIG_DFL);
  std::signal(SIGTERM, SIG_DFL);
  std::signal(SIGQUIT, SIG_DFL);
}

void Indexer::AddWordOccurrence(
    const std::string& word,
    const std::string& filename,
    const std::string& link)
{
  file_cache_.AddToFile(word, FilenameAndLink(filename, link));
}

void Indexer::IndexFile(const std::string& filename)
{
  if (indexed_files_.container.count({filename, ""}))
  {
    return;
  }
  std::ifstream file(filename);
  if (!file.good())
  {
    return;
  }

  std::string link;
  std::getline(file, link);
  while (file.good())
  {
    std::string word;
    char ch;
    while (file.get(ch) && isalnum(ch))
    {
      word.append(1, tolower(ch));
    }
    if (word.size() > 3)
    {
      AddWordOccurrence(word, filename, link);
    }
  }
  indexed_files_.container.insert({filename, link});
}
