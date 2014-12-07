#ifndef FILENAMEANDLINK_H
#define FILENAMEANDLINK_H

#include <string>
#include <iostream>
#include <functional>

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

#endif
