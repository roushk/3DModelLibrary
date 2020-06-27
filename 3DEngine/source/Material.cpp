#include <pch.h>
#include "Material.h"

vec3 Material::getPixelRGB(int x, int y)
{
  return pixels.at(x + (y * width));
}



bool MaterialReader::ReadMaterial(std::string filename)
{
  auto start = std::chrono::system_clock::now();
  Material object;
  std::cout << "Loading Material File: " << filename << std::endl;

  //reads entire file into string at once
  std::string data;
  std::stringstream data2stream;
  /*
  std::string data2;
  file.seekg(0, std::ios::end);
  data.reserve(file.tellg()); //allocated all memory for it
  file.seekg(0, std::ios::beg);
  data.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
  */

  ReadinFile(filename, data2stream);
  data = data2stream.str();
  unsigned comment = data.find('#'); //sets comment pos

  //strips comments
  //for some reason was not breaking when comment was npos so added the -1
  while (comment != std::string::npos && comment != -1)
  {
    unsigned newLine = data.find('\n', comment);

    if (newLine == std::string::npos || newLine == -1)
      throw(std::string("No newline for end of comment in Material Reader for Material:") + filename);

    //erases the comment from pos comment of size newline pos - comment pos
    data.erase(comment, newLine - comment + 1);
    comment = data.find("#"); //relooks for another comment
  }

  for (auto& c : data) //for each char c in data
  {
    if (c == '\n')
      c = ' '; //replaces all newlines with spaces
  }

  //now 4 locations in order with magic number, width, height, max value
  //std::cout << data;

  unsigned endOfData = data.find(' ', 1);
  object.magicNum = data.substr(0, endOfData);
  data.erase(0, endOfData + 1);

  endOfData = data.find(' ', 1);
  object.width = std::stoi(data.substr(0, endOfData));
  data.erase(0, endOfData + 1);

  endOfData = data.find(' ', 1);
  object.height = std::stoi(data.substr(0, endOfData));
  data.erase(0, endOfData + 1);

  endOfData = data.find(' ', 1);
  object.maxColorVal = std::stoi(data.substr(0, endOfData));
  data.erase(0, endOfData + 1);

  std::vector<int> pixelData;
  pixelData.reserve(object.width * object.height * 3);

  //Here needs to be optimized (heck yeah so fast)
  //if each of 3 isnt space put into int;
  //atoi using manual add of chars - 48
  int offset = 0;
  while (data[offset] != NULL)
  {
    int newVal[3];
    int newValEnd = 0;
    int toDelete = 0;
    if (data[offset] != ' ')
    {
      newVal[0] = data[offset] - 48;
      toDelete = 1;

      if (data[offset + 1] != ' ')
      {
        ++toDelete;
        newVal[1] = data[offset + 1] - 48;

        if (data[offset + 2] != ' ')
        {
          ++toDelete;
          newVal[2] = data[offset + 2] - 48;
        }
      }
      if (toDelete == 1)
        newValEnd = newVal[0];
      if (toDelete == 2)
      {
        newValEnd = 10 * newVal[0] + newVal[1];
      }
      if (toDelete == 3)
      {
        newValEnd = 100 * newVal[0] + 10 * newVal[1] + newVal[2];
      }
    }
    pixelData.push_back(newValEnd);
    offset += toDelete + 1;
  }

  object.pixels.reserve(object.width * object.height);
  //std::cout << data;
  for (int i = 0; i < pixelData.size() - 2; i += 3)
  {
    object.pixels.push_back({ pixelData[i] / 255.0f, pixelData[i + 1] / 255.0f, pixelData[i + 2] / 255.0f });
  }
  //std::cout << data;

  auto end = std::chrono::system_clock::now();
  auto chronoDt = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  object.timeToRead = chronoDt.count();

  std::cout << "Time to Read Material File: " << filename << " Took: " << object.timeToRead << " Milliseconds" << std::
    endl;
  materials.push_back(object);
  return true;
}

Material& MaterialReader::GetMaterial(int location)
{
  return materials[location];
}

unsigned MaterialReader::MaxMaterials()
{
  return materials.size();
}
