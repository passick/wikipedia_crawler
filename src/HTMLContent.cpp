#include <string>
#include <vector>

#include "HTMLContent.h"
#include "HTMLTag.h"

HTMLContent::HTMLContent()
{ }

HTMLContent::HTMLContent(const HTMLContent& content)
{
  *this = content;
}

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
        HTMLTag *tag = new HTMLTag;
        *tag = HTMLTag(parse_from, index, &index);
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

HTMLContent::~HTMLContent()
{
  for (HTMLTag *&tag : tags_)
  {
    delete tag;
    tag = nullptr;
  }
}

HTMLContent& HTMLContent::operator=(const HTMLContent& content)
{
  text_ = content.text_;
  tags_.reserve(content.tags_.size());
  for (HTMLTag *tag : content.tags_)
  {
    HTMLTag *new_tag = new HTMLTag;
    *new_tag = *tag;
    tags_.push_back(new_tag);
  }
  return *this;
}

const std::vector<std::string>& HTMLContent::text() const
{
  return text_;
}

const std::vector<HTMLTag*>& HTMLContent::tags() const
{
  return tags_;
}
