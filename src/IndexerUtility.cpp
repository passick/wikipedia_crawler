#include <string>

#include "Indexer.h"

int main()
{
  Indexer wiki_indexer("./data2/",
      "./index/",
      "./index/indexed_files");
  wiki_indexer.StartIndexing();

  return 0;
}
