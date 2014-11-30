#include <vector>
#include <iostream>

#include "Crawler.h"
#include "HTMLContent.h"

int main()
{
  //std::vector<char> banned_symbols = { ':' };
  //Crawler wiki_crawler("./data/",
      //"http://simple.wikipedia.org",
      //"/wiki",
      //banned_symbols);
  //wiki_crawler.Crawl("/wiki/Main_Page");
  std::ifstream file("data/1416675704 1804289383");
  std::string content, tmp;
  while (std::getline(file, tmp))
  {
    content += tmp;
    content.push_back('\n');
  }
  HTMLContent yahoo(content, 0);
  HTMLTag tag = yahoo.tags()[1];
  const HTMLTag *body2 = HTMLTag::traverse_path(tag, {
      { { "__name__", {"body"} } },
      { { "id", {"content"} } },
      { { "id", {"bodyContent"} } },
      { { "id", {"mw-content-text"} } }
      });
  std::cout << body2->get_text() << std::endl;

  //std::string text = yahoo.tags()[1].get_text();
  //std::cout << text << std::endl;

  return 0;
}
