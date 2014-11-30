#include <string>

#include "HTMLContent.h"
#include "HTMLTag.h"
#include "WikiPage.h"

WikiPage::WikiPage(const HTMLTag& tag)
{
  const HTMLTag *content_tag = HTMLTag::traverse_path(tag, {
      { { "__name__", {"body"} } },
      { { "id", {"content"} } }
      });
  const HTMLTag *title_tag = HTMLTag::traverse_path(*content_tag, {
      { { "id", {"firstHeading"} } }
      });
  title_ = title_tag->get_text();
  const HTMLTag *text_tag = HTMLTag::traverse_path(*content_tag, {
      { { "id", {"bodyContent"} } },
      { { "id", {"mw-content-text"} } }
      });
  text_ = text_tag->get_text();
  std::vector<HTMLTag*> all_links =
    text_tag->get_all_descendants({ { "__name__", {"a"} } });
  for (HTMLTag* link : all_links)
  {
    const HTMLTag::Attribute *attribute = link->get_attribute("href");
    if (!attribute)
    {
      continue;
    }
    if (is_link_ok(attribute->value()))
    {
      links_.push_back(transform_link(attribute->value()));
    }
  }
}

const std::string& WikiPage::title()
{
  return title_;
}

const std::string& WikiPage::text()
{
  return text_;
}

const std::vector<std::string>& WikiPage::links()
{
  return links_;
}

bool WikiPage::is_link_ok(const std::string& link)
{
  // If link does not begin with /wiki, we are ignoring it
  std::string wiki = "/wiki";
  if (link.compare(0, wiki.length(), wiki))
  {
    return false;
  }
  size_t colon_index = link.find(':');
  if (colon_index != std::string::npos)
  {
    std::string allowed_namespace = "Category";
    if (!link.compare(wiki.length(),
          allowed_namespace.length(), allowed_namespace))
    {
      return true;
    }
    return false;
  }
  return true;
}

std::string WikiPage::transform_link(const std::string& link)
{
  size_t hash_index = link.find('#');
  if (hash_index == std::string::npos)
  {
    return link;
  }
  return link.substr(0, hash_index + 1);
}
