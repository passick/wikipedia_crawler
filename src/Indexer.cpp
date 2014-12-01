#include <string>

#include "Indexer.h"

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

bool FilenameAndLink::operator<(const FilenameAndLink& rhs) const
{
  return filename < rhs.filename;
}

bool FilenameAndLink::operator==(const FilenameAndLink& rhs) const
{
  return filename == rhs.filename && link == rhs.link;
}

std::ostream& operator<<(
    std::ostream& stream, const FilenameAndLink& object)
{
  stream << "\"" << object.filename << "\" " << object.link << std::endl;
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

Indexer::Indexer(const std::string& files_directory,
      const std::string& index_directory,
      const std::string& list_of_indexed_files) :
  files_directory_(files_directory),
  index_directory_(index_directory)
{
  indexed_files_ = SaveableStringContainer<std::unordered_set<FilenameAndLink>>(list_of_indexed_files);
}

size_t std::hash<FilenameAndLink>::operator()(const FilenameAndLink& obj) const
{
  return std::hash<std::string>()(obj.filename);
}
