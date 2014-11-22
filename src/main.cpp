#include "Crawler.h"
#include <vector>

int main()
{
  std::vector<char> banned_symbols = { ':' };
  Crawler wiki_crawler("./data/",
      "http://simple.wikipedia.org",
      "/wiki",
      banned_symbols);
  wiki_crawler.Crawl("/wiki/Main_Page");

  return 0;
}
