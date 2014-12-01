#ifndef SAVEABLESTRINGCONTAINER_CPP
#define SAVEABLESTRINGCONTAINER_CPP

#include <string>
#include <fstream>
#include "SaveableStringContainer.h"

template <class T>
SaveableStringContainer<T>::SaveableStringContainer(
    const std::string& filename) :
  filename_(filename)
{
  if (filename_ != "")
    Load();
}

template <class T>
void SaveableStringContainer<T>::Load()
{
  std::ifstream file(filename_);
  if (!file)
  {
    return;
  }
  std::string line;
  while (std::getline(file, line))
  {
    container.insert(container.end(), (typename T::value_type)(line));
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
