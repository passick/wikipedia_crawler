#ifndef HTMLCONTENT_H
#define HTMLCONTENT_H

#include <string>
#include <vector>

#include "HTMLTag.h"

class HTMLTag;

class HTMLContent
{
  std::vector<std::string> text_;
  std::vector<HTMLTag*> tags_;

 public:
  HTMLContent();
  HTMLContent(const HTMLContent& content);
  HTMLContent(const std::string& parse_from,
      int start_index, int *ended_at = NULL);

  ~HTMLContent();

  HTMLContent& operator=(const HTMLContent& content);

  const std::vector<std::string>& text() const;
  const std::vector<HTMLTag*>& tags() const;
};

#endif
