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
  HTMLTag *head = tag.get_descendant({{"__name__", {"head"}}});
  //std::string text = yahoo.tags()[1].get_text();
  //std::cout << text << std::endl;

  return 0;
}
