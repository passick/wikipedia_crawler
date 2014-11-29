#ifndef HTMLCONTENT_H
#define HTMLCONTENT_H

#include <string>
#include <vector>

#include "HTMLTag.h"

class HTMLContent
{
  std::vector<std::string> text_;
  std::vector<HTMLTag> tags_;

 public:
  HTMLContent();
  HTMLContent(const std::string& parse_from,
      int start_index, int *ended_at = NULL);
};

#endif
