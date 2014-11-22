#ifndef SAVEABLESTRINGCONTAINER_H
#define SAVEABLESTRINGCONTAINER_H

#include <string>

template <class T>
class SaveableStringContainer
{
  std::string filename_;
 public:
  T container;
  SaveableStringContainer(const std::string& filename);
  void Load(const std::string& load_from);
  void Save();
  ~SaveableStringContainer();
};

#include "SaveableStringContainer.cpp"

#endif
