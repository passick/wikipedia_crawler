#include <cstring>
#include <dirent.h>

#include <string>
#include <fstream>
#include <algorithm>

#include "Indexer.h"

volatile sig_atomic_t Indexer::indexer_terminated = false;
const int Indexer::not_saved_indexes_limit_ = 5;

FilenameAndLink::FilenameAndLink()
{ }

FilenameAndLink::FilenameAndLink(const std::string& string)
{
  size_t closing_quote_index = string.find('"', 1);
  if (string[0] != '"' || closing_quote_index == std::string::npos)
  {
    return;
  }
  filename = string.substr(1, closing_quote_index - 1);
  link = string.substr(closing_quote_index + 2, std::string::npos);
}

FilenameAndLink::FilenameAndLink(const std::string& filename,
    const std::string& link) :
  filename(filename), link(link)
{ }

bool FilenameAndLink::operator<(const FilenameAndLink& rhs) const
{
  return filename < rhs.filename;
}

bool FilenameAndLink::operator==(const FilenameAndLink& rhs) const
{
  return filename == rhs.filename;
}

std::ostream& operator<<(
    std::ostream& stream, const FilenameAndLink& object)
{
  stream << "\"" << object.filename << "\" " << object.link;
  return stream;
}

std::istream& operator>>(std::istream& stream,
    FilenameAndLink& object)
{
  std::string tmp;
  std::getline(stream, tmp);
  object = FilenameAndLink(tmp);
  return stream;
}

size_t std::hash<FilenameAndLink>::operator()(const FilenameAndLink& obj) const
{
  return std::hash<std::string>()(obj.filename);
}

Indexer::Indexer(const std::string& files_directory,
      const std::string& index_directory,
      const std::string& list_of_indexed_files) :
  files_directory_(files_directory),
  index_directory_(index_directory)
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

  for (auto word_indexes_pair : not_saved_indexes_)
  {
    SaveWordIndex(word_indexes_pair.first, false);
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
  not_saved_indexes_[word].push_back(FilenameAndLink(filename, link));
  if (not_saved_indexes_[word].size() >= not_saved_indexes_limit_)
  {
    SaveWordIndex(word);
  }
}

void Indexer::SaveWordIndex(
    const std::string& word,
    bool delete_from_not_saved)
{
  std::ofstream word_file(index_directory_ + word,
      std::ofstream::out | std::ofstream::app);
  if (!word_file.good())
  {
    return;
  }
  for (const FilenameAndLink& index : not_saved_indexes_[word])
  {
    word_file << index;
  }
  if (delete_from_not_saved)
  {
    not_saved_indexes_.erase(word);
  }
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
