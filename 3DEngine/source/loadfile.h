#pragma once
#include "pch.h"
#include <string>
#include <fstream>

inline std::string load_file(const std::string& filename)
{
  std::ifstream file(filename);


  std::string data;

  file.seekg(0, std::ios::end);
  data.reserve(static_cast<const unsigned>(file.tellg()));  //allocated all memory for it
  file.seekg(0, std::ios::beg);

  data.assign(std::istreambuf_iterator<char>(file),
    std::istreambuf_iterator<char>());

  return data;
}
