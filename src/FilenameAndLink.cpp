#include "FilenameAndLink.h"

FilenameAndLink::FilenameAndLink()
{ }

FilenameAndLink::FilenameAndLink(const std::string& string)
{
  size_t closing_quote_index = string.find('"', 1);
  if (string[0] != '"' || closing_quote_index == std::string::npos)
  {
    return;
  }
  filename = parse_filename(string.substr(1, closing_quote_index - 1));
  link = string.substr(closing_quote_index + 2, std::string::npos);
}

FilenameAndLink::FilenameAndLink(const std::string& filename,
    const std::string& link) :
  link(link), filename(parse_filename(filename))
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

std::string FilenameAndLink::parse_filename(const std::string& filename)
{
  std::string::size_type file_extension_index = filename.rfind('.');
  return filename.substr(0, file_extension_index);
}

size_t std::hash<FilenameAndLink>::operator()(const FilenameAndLink& obj) const
{
  return std::hash<std::string>()(obj.filename);
}

