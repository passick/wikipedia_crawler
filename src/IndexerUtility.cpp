#include <string>

#include "Indexer.h"

int main()
{
  Indexer wiki_indexer("./data3/",
      "./index/",
      "./index/indexed_files");
  wiki_indexer.StartIndexing();

  return 0;
}
