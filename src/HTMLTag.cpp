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

const std::vector<HTMLTag*> HTMLTag::direct_descendant_tags() const
{
  if (!content_)
  {
    return std::vector<HTMLTag*>();
  }
  return content_->tags();
}

const HTMLTag::Attribute *HTMLTag::get_attribute(const std::string& name) const
{
  for (const HTMLTag::Attribute& attribute : attributes_)
  {
    if (attribute.name() == name)
    {
      return &attribute;
    }
  }
  return nullptr;
}

bool HTMLTag::matches_properties(
      const std::unordered_map<std::string, std::unordered_set<std::string> >&
        properties) const
{
  bool tag_matches = true;
  for (auto& property : properties)
  {
    std::string property_name = property.first;
    const std::unordered_set<std::string>& possible_values = property.second;
    if (property_name == "__name__")
    {
      tag_matches = false;
      for (const std::string& name : possible_values)
      {
        if (name == this->name_)
        {
          tag_matches = true;
          break;
        }
      }
      if (tag_matches)
      {
        continue;
      }
      else
      {
        break;
      }
    }
    const std::unordered_set<std::string>& existing_values =
      (this->attributes_map_.count(property_name) ?
       this->attributes_map_.at(property_name) :
       std::unordered_set<std::string>());
    tag_matches = false;
    for (const std::string& value : possible_values)
    {
      if (existing_values.count(value))
      {
        tag_matches = true;
        break;
      }
    }
    if (!tag_matches)
    {
      break;
    }
  }
  return tag_matches;
}

std::string HTMLTag::get_text() const
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
      result += content_->tags()[i]->get_text();
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
  for (HTMLTag *tag : content_->tags())
  {
    if (tag->matches_properties(required_properties))
    {
      return tag;
    }
  }
  return nullptr;
}

// Finds all (not only direct) descendant tags matching given properties.
std::vector<HTMLTag*> HTMLTag::get_all_descendants(
    const std::unordered_map<std::string, std::unordered_set<std::string> >&
      required_properties) const
{
  std::vector<HTMLTag*> result;
  for (HTMLTag* descendant : direct_descendant_tags())
  {
    if (descendant->matches_properties(required_properties))
    {
      result.push_back(descendant);
    }
    for (HTMLTag* matching_descendant :
        descendant->get_all_descendants(required_properties))
    {
      result.push_back(matching_descendant);
    }
  }
  return result;
}

const HTMLTag* HTMLTag::traverse_path(
    const HTMLTag& start,
    std::initializer_list<std::unordered_map<std::string,
        std::unordered_set<std::string> > > path)
{
  const HTMLTag *current = &start;
  for (auto next_tag : path)
  {
    current = current->get_descendant(next_tag);
    if (!current)
    {
      break;
    }
  }
  return current;
}
