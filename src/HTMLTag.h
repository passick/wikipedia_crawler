#ifndef HTMLTAG
#define HTMLTAG

#include <string>
#include <vector>

class HTMLContent;

class HTMLTag
{
  class Attribute
  {
    static const std::string illegal_characters;
    std::string name_;
    std::string value_;

    bool is_char_legal(char ch);
   public:
    Attribute(const std::string& parse_from,
        int start_index, int *ended_at);
  };
  std::string name_;
  std::vector<Attribute> attributes_;
  HTMLContent *content_;

 public:
  HTMLTag();
  HTMLTag(const HTMLTag& tag);
  HTMLTag(const std::string& parse_from,
      int start_index, int *ended_at);
  ~HTMLTag();
  HTMLTag& operator=(const HTMLTag& tag);

  std::string get_text();
};

#endif
