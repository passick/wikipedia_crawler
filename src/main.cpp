#include <vector>
#include <iostream>

#include "Crawler.h"
#include "WikiPage.h"
#include "Indexer.h"

int main()
{
  std::vector<char> banned_symbols = { ':' };
  Crawler wiki_crawler("./data2/",
      "http://simple.wikipedia.org",
      "/wiki",
      banned_symbols);
  wiki_crawler.Crawl("/wiki/Main_Page");
  //Indexer wiki_indexer("./data2/",
      //"./index/",
      //"./index/indexed_files");
  //wiki_indexer.StartIndexing();
  
  return 0;
}
