
#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

using namespace glm;

#define MAXLINELENGTH 10000

struct Material
{
  //0,0 to width - 1,height - 1
  vec3 getPixelRGB(int x, int y);

  std::vector<vec3> pixels;

  std::string name;
  std::string magicNum;
  int width = 0;
  int height = 0;
  int maxColorVal = 255;
  float timeToRead;

  vec3 emissiveColor = vec3(0, 0, 0);
  vec3 ambientColor = vec3(0, 0, 0);
};

class MaterialReader
{
public:
  bool ReadMaterial(std::string filename);
  bool ReadMaterialPNG(std::string filename);
  Material& GetMaterial(int location);
  unsigned MaxMaterials();
private:

  std::vector<Material> materials;
};

inline void ReadinFile(std::string const& filename, std::stringstream& result)
{
  std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
  std::ifstream::pos_type pos = ifs.tellg();

  char * buf = new char[pos];
  ifs.seekg(0, std::ios::beg);
  ifs.read(buf, pos);
  result.write(buf, pos);
  delete[]buf;
}

#endif


