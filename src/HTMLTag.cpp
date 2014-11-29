#include <cctype>

#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "HTMLTag.h"
#include "HTMLContent.h"

class HTMLContent;

const std::string
  HTMLTag::Attribute::illegal_characters = "\"'>/=\t\n\v\f\r ";

bool HTMLTag::Attribute::is_char_legal(char ch)
{
  for (auto illegal_char : illegal_characters)
  {
    if (ch == illegal_char)
    {
      return false;
    }
  }
  return true;
}
HTMLTag::Attribute::Attribute(const std::string& parse_from,
    int start_index, int *ended_at = nullptr)
{
  // https://html.spec.whatwg.org/multipage/syntax.html#attributes-2
  int index = start_index;
  while (index < parse_from.length() && !is_char_legal(parse_from[index]))
  {
    index++;
  }
  while (index < parse_from.length() && is_char_legal(parse_from[index]))
  {
    name_.append(1, parse_from[index]);
    index++;
  }
  while (index < parse_from.length() && isspace(parse_from[index]))
  {
    index++;
  }
  if (index < parse_from.length() && parse_from[index] != '=')
  {
    if (ended_at)
    {
      *ended_at = index;
    }
    return;
  }
  index++;
  while (index < parse_from.length() && isspace(parse_from[index]))
  {
    index++;
  }
  if (parse_from[index] == '\'' || parse_from[index] == '"')
  {
    char closing_char = parse_from[index];
    index++;
    while (index < parse_from.length() &&
        parse_from[index] != closing_char)
    {
      value_.append(1, parse_from[index]);
      index++;
    }
    index++;
  }
  else
  {
    while (index < parse_from.length() && is_char_legal(parse_from[index]))
    {
      value_.append(1, parse_from[index]);
      index++;
    }
  }
  while (index < parse_from.length() && isspace(parse_from[index]))
  {
    index++;
  }
  if (ended_at != nullptr)
  {
    *ended_at = index;
  }
}

const std::string& HTMLTag::Attribute::name() const
{
  return name_;
}

const std::string& HTMLTag::Attribute::value() const
{
  return value_;
}

HTMLTag::HTMLTag()
{
  content_ = nullptr;
}

HTMLTag::HTMLTag(const HTMLTag& tag)
{
  content_ = nullptr;
  *this = tag;
}

HTMLTag::HTMLTag(const std::string& parse_from, int start_index, int *ended_at = nullptr)
{
  content_ = nullptr;
  int index = start_index;
  while (index < parse_from.length() && parse_from[index] != '<')
  {
    index++;
  }
  index++;
  // skip comments
  // (I am not realy following the syntax for comments here, but still...)
  // https://html.spec.whatwg.org/multipage/syntax.html#comments
  if (index < parse_from.length() && parse_from[index] == '!')
  {
    while (index < parse_from.length() && parse_from[index] != '>')
    {
      index++;
    }
    index++;
    if (ended_at)
    {
      *ended_at = index;
    }
    return;
  }
  // https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-name
  while (index < parse_from.length() &&
      (isalnum(parse_from[index]) || parse_from[index] == '!'))
  {
    name_.append(1, parse_from[index]);
    index++;
  }
  while (index < parse_from.length() && isspace(parse_from[index]))
  {
    index++;
  }
  while (index < parse_from.length() &&
      parse_from[index] != '>' && parse_from[index] != '/')
  {
    HTMLTag::Attribute attribute = Attribute(parse_from, index, &index);
    attributes_.push_back(attribute);
    attributes_map_[attribute.name()].insert(attribute.value());
  }
  if (index >= parse_from.length() || parse_from[index] == '/')
  {
    index += 2;
    if (ended_at)
    {
      *ended_at = std::min(index, (int)parse_from.length());
    }
    return;
  }
  index++;
  content_ = new HTMLContent;
  *content_ = HTMLContent(parse_from, index, &index);
  // https://html.spec.whatwg.org/multipage/syntax.html#end-tags
  while (index < parse_from.length() && parse_from[index] != '>')
  {
    index++;
  }
  if (index < parse_from.length())
  {
    index++;
  }
  if (ended_at)
  {
    *ended_at = index;
  }
}

HTMLTag::~HTMLTag()
{
  delete content_;
  content_ = nullptr;
}

HTMLTag& HTMLTag::operator=(const HTMLTag& tag)
{
  content_ = nullptr;
  name_ = tag.name_;
  attributes_ = tag.attributes_;
  attributes_map_ = tag.attributes_map_;
  if (tag.content_ != nullptr)
  {
    content_ = new HTMLContent;
    *content_ = *(tag.content_);
  }
  return *this;
}

std::string HTMLTag::get_text()
{
  std::string result;
  if (!content_)
  {
    return "";
  }
  for (int i = 0; i < content_->text().size(); ++i)
  {
    result += content_->text()[i];
    if (i < content_->tags().size())
    {
      result += content_->tags()[i].get_text();
    }
  }
  return result;
}

// Return direct descendant with required properties.
//
// Returned value (if it is not nullptr) will have for each property
// at least one attribute that matches it.
//
// Keys with special meanings:
// __name__ : name of required tag.
HTMLTag* HTMLTag::get_descendant(
    const std::unordered_map<std::string, std::unordered_set<std::string> >&
      required_properties) const
{
  if (!content_)
  {
    return nullptr;
  }
  for (HTMLTag& tag : content_->tags())
  {
    bool tag_fits = true;
    for (auto& property : required_properties)
    {
      std::string property_name = property.first;
      const std::unordered_set<std::string>& possible_values = property.second;
      if (property_name == "__name__")
      {
        tag_fits = false;
        for (const std::string& name : possible_values)
        {
          if (name == tag.name_)
          {
            tag_fits = true;
            break;
          }
        }
        if (tag_fits)
        {
          continue;
        }
        else
        {
          break;
        }
      }
      const std::unordered_set<std::string>& existing_values =
        (attributes_map_.count(property_name) ?
         attributes_map_.at(property_name) :
         std::unordered_set<std::string>());
      tag_fits = false;
      for (const std::string& value : possible_values)
      {
        if (existing_values.count(value))
        {
          tag_fits = true;
          break;
        }
      }
      if (!tag_fits)
      {
        break;
      }
    }
    if (tag_fits)
    {
      return &tag;
    }
  }
  return nullptr;
}
