#include <string>
#include <vector>

#include "HTMLContent.h"
#include "HTMLTag.h"

HTMLContent::HTMLContent()
{ }

HTMLContent::HTMLContent(const std::string& parse_from,
    int start_index, int *ended_at)
{
  int index = start_index;
  while (index < parse_from.length())
  {
    text_.push_back("");
    while (index < parse_from.length() &&
        parse_from[index] != '<')
    {
      text_.back().append(1, parse_from[index]);
      index++;
    }
    if (index < parse_from.length())
    {
      if (parse_from[index + 1] != '/')
      {
        HTMLTag tag = HTMLTag(parse_from, index, &index);
        tags_.push_back(tag);
      }
      else
      {
        break;
      }
    }
  }
  if (ended_at)
  {
    *ended_at = index;
  }
}

std::vector<std::string>& HTMLContent::text()
{
  return text_;
}

std::vector<HTMLTag>& HTMLContent::tags()
{
  return tags_;
}
