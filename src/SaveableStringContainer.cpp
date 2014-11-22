#ifndef SAVEABLESTRINGCONTAINER_CPP
#define SAVEABLESTRINGCONTAINER_CPP

#include <string>
#include <fstream>
#include "SaveableStringContainer.h"

template <class T>
SaveableStringContainer<T>::SaveableStringContainer(
    const std::string& filename = "") :
  filename_(filename)
{
  if (filename_ != "")
    Load(filename_);
}

template <class T>
void SaveableStringContainer<T>::Load(const std::string& load_from)
{
  std::ifstream file(load_from);
  if (!file)
  {
    return;
  }
  std::string line;
  while (std::getline(file, line))
  {
    container.insert(container.end(), line);
  }
}

template <class T>
void SaveableStringContainer<T>::Save()
{
  std::ofstream file(filename_);
  for (auto element : container)
  {
    file << element << std::endl;
  }
}

template <class T>
SaveableStringContainer<T>::~SaveableStringContainer()
{
  Save();
}

#endif
