#include <string>

#include "HTMLContent.h"
#include "HTMLTag.h"
#include "WikiPage.h"

WikiPage::WikiPage(HTMLTag& tag)
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
    if (!attribute->value().compare(0, 5, "/wiki"))
    {
      links_.push_back(attribute->value());
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
