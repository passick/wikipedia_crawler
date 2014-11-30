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
}

const std::string& WikiPage::title()
{
  return title_;
}

const std::string& WikiPage::text()
{
  return text_;
}
