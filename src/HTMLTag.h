#ifndef HTMLTAG
#define HTMLTAG

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class HTMLContent;

class HTMLTag
{
 public:
  class Attribute
  {
    static const std::string illegal_characters;
    std::string name_;
    std::string value_;

    bool is_char_legal(char ch);

   public:
    Attribute(const std::string& parse_from,
        int start_index, int *ended_at);
    const std::string& name() const;
    const std::string& value() const;
  };

 private:
  std::string name_;
  std::vector<Attribute> attributes_;
  std::unordered_map<std::string, std::unordered_set<std::string> >
    attributes_map_;
  HTMLContent *content_;

  bool matches_properties(
      const std::unordered_map<std::string, std::unordered_set<std::string> >&
        properties) const;

 public:
  HTMLTag();
  HTMLTag(const HTMLTag& tag);
  HTMLTag(const std::string& parse_from,
      int start_index, int *ended_at);
  ~HTMLTag();
  HTMLTag& operator=(const HTMLTag& tag);

  const std::string& name();
  const std::vector<HTMLTag*> direct_descendant_tags() const;
  const Attribute* get_attribute(const std::string& name) const;

  std::string get_text() const;
  HTMLTag* get_descendant(
      const std::unordered_map<std::string, std::unordered_set<std::string> >&
        required_properties) const;
  std::vector<HTMLTag*> get_all_descendants(
    const std::unordered_map<std::string, std::unordered_set<std::string> >&
      required_properties) const;

  static const HTMLTag* traverse_path(
      const HTMLTag& start,
      std::initializer_list<std::unordered_map<std::string,
          std::unordered_set<std::string> > > path);
};

#endif
