#include <pch.h>
#include "Object.h"
#include "singleton.h"
#include "loadfile.h"


Model::Model(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<Index>& indices,
  const std::vector<glm::vec2>& cyl, const std::vector<glm::vec2>& sphere,
  const std::vector<glm::vec2>& planar) : name(name), vertices(vertices), indices(indices),
  uvCylindrical(cyl), uvSpherical(sphere), uvPlanar(planar)
{
  uvSetting = MeshUVSetting::sphere;
  setup_mesh();
}

Model::Model(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<Index>& indices) :name(name), vertices(vertices), indices(indices)
{
  uvSetting = MeshUVSetting::custom;
  setup_mesh();
}

Model::Model(const Model& rhs)
{
  *this = rhs;

}

Model& Model::operator=(const Model& rhs)
{
  modelMatrix = rhs.modelMatrix;
  meshes = rhs.meshes;
  halfExtents = rhs.halfExtents;
  name = rhs.name;;
  vertices = rhs.vertices;
  indices = rhs.indices;
  uvSetting = rhs.uvSetting;
  uvCylindrical = rhs.uvCylindrical;
  uvSpherical = rhs.uvSpherical;
  uvPlanar = rhs.uvPlanar;
  meshMatrix = rhs.meshMatrix;
  vao = rhs.vao;
  vbo = rhs.vbo;
  ibo = rhs.ibo;
  maxSize = rhs.maxSize;
  minSize = rhs.minSize;
  return *this;
}


VAO Model::get_vao() const
{
  return vao;
}

VBO Model::get_vbo() const
{
  return vbo;
}

IBO Model::get_ibo() const
{
  return ibo;
}

void Model::change_uv_coord_mapping(MeshUVSetting newSetting)
{
  uvSetting = newSetting;
  if (uvSetting == MeshUVSetting::custom)
    throw(
      "UV setting cannot be set to custom, has to have custom UV coordinates loaded in at starting. check the mesh to make sure all verts have UV coords"
    );
  for (unsigned i = 0; i < vertices.size(); ++i)
  {
    if (uvSetting == MeshUVSetting::cylinder)
      vertices[i].texCoords = uvCylindrical[i];
    if (uvSetting == MeshUVSetting::sphere)
      vertices[i].texCoords = uvSpherical[i];
    if (uvSetting == MeshUVSetting::planar)
      vertices[i].texCoords = uvPlanar[i];
  }

  //need to re bind tex coords
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof Vertex, vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof Index, indices.data(), GL_STATIC_DRAW);
  // tex coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, texCoords)));

  glBindVertexArray(0); // so other commands don't accidentally mess up our vao
}

void Model::setup_mesh()
{
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ibo);
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof Vertex, vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof Index, indices.data(), GL_STATIC_DRAW);
  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, nullptr);
  // normals
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, normal)));
  // tex coords
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, texCoords)));
  // tangent
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, tangent)));
  // bitangent
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof Vertex, reinterpret_cast<void*>(offsetof(Vertex, bitangent)));

  glBindVertexArray(0); // so other commands don't accidentally mess up our vao
}

Model ObjectReader::load_model(const std::string& path)
{
  Assimp::Importer importer;
  Model newModel;
  const auto scene = importer.ReadFile(path.c_str(),
    //aiProcessPreset_TargetRealtime_Quality |
    //aiProcess_OptimizeMeshes |
    aiProcess_GenSmoothNormals |   
    aiProcess_CalcTangentSpace |  //cant do that cause assimp is to stupid to understand
    //that it needs to run the gen uv coords and smooth normals BEFORE it greates the tangent space 
    //normals....
    //aiProcess_GenNormals |
    //aiProcess_JoinIdenticalVertices |
    aiProcess_GenUVCoords |
    aiProcess_SortByPType |
    aiProcess_PreTransformVertices);
  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
  {
    std::cout << "couldn't load model from path: " << path << std::endl;
  }
  
  newModel.name = scene->mRootNode->mName.C_Str();

  process_node(scene->mRootNode, scene, newModel);
  //Process materials and generate UV coordinates

  return newModel;
}

void ObjectReader::process_node(aiNode* node, const aiScene* scene, Model& m)
{
  for (unsigned i = 0; i < node->mNumMeshes; ++i)
  {
    const auto mesh = scene->mMeshes[node->mMeshes[i]];
    process_mesh(mesh, scene, m);
    m.change_uv_coord_mapping(MeshUVSetting::planar);
  }
  for (unsigned i = 0; i < node->mNumChildren; ++i)
  {
    process_node(node->mChildren[i], scene, m);
  }
}

glm::vec2 ObjectReader::uv_calc(glm::vec3 point)
{
  float absX = abs(point.x);
  float absY = abs(point.y);
  float absZ = abs(point.z);

  bool XPositive = point.x > 0 ? true : false;
  bool YPositive = point.y > 0 ? true : false;
  bool ZPositive = point.z > 0 ? true : false;

  float largestLine = 0;
  glm::vec2 uv{ 0,0 };

  //posx, negx, posy, negy, posz, negz
  if (XPositive && absX >= absY && absX >= absZ)
  {

    largestLine = absX;
    uv.x = -point.z;
    uv.y = point.y;
  }
  if (XPositive == false && absX >= absY && absX >= absZ)
  {

    largestLine = absX;
    uv.x = point.z;
    uv.y = point.y;
  }
  if (YPositive && absY >= absX && absY >= absZ)
  {

    largestLine = absY;
    uv.x = point.x;
    uv.y = -point.z;
  }
  if (YPositive == false && absY >= absX && absY >= absZ)
  {

    largestLine = absY;
    uv.x = point.x;
    uv.y = point.z;
  }
  if (ZPositive && absZ >= absX && absZ >= absY)
  {

    largestLine = absZ;
    uv.x = point.x;
    uv.y = point.y;
  }
  if (ZPositive == false && absZ >= absX && absZ >= absY)
  {

    largestLine = absZ;
    uv.x = -point.x;
    uv.y = point.y;
  }

  uv.x = 0.5f * (uv.x / largestLine + 1.0f);
  uv.y = 0.5f * (uv.y / largestLine + 1.0f);
  return uv;
}

void ObjectReader::process_mesh(aiMesh* mesh, const aiScene* scene, Model& m)
{
  //if (mesh->mNumVertices <= 2 || mesh->mNumFaces <= 0)
  //  return;

  std::string name(mesh->mName.C_Str());

  glm::vec3 maxSize(std::numeric_limits<float>::min());
  glm::vec3 minSize(std::numeric_limits<float>::max());
  //bool texCoordsExist = true;
  std::vector<Vertex> vertices;

  vertices.reserve(mesh->mNumVertices);

  for (unsigned i = 0; i < mesh->mNumVertices; ++i)
  {
    Vertex vert;
    vert.pos.x = mesh->mVertices[i].x;
    vert.pos.y = mesh->mVertices[i].y;
    vert.pos.z = mesh->mVertices[i].z;
    vert.normal.x = mesh->mNormals[i].x;
    vert.normal.y = mesh->mNormals[i].y;
    vert.normal.z = mesh->mNormals[i].z;

    vert.tangent.x = mesh->mTangents[i].x;
    vert.tangent.y = mesh->mTangents[i].y;
    vert.tangent.z = mesh->mTangents[i].z;
    
    vert.bitangent.x = mesh->mBitangents[i].x;
    vert.bitangent.y = mesh->mBitangents[i].y;
    vert.bitangent.z = mesh->mBitangents[i].z;


    if (vert.pos.x > maxSize.x)
      maxSize.x = vert.pos.x;
    if (vert.pos.y > maxSize.y)
      maxSize.y = vert.pos.y;
    if (vert.pos.z > maxSize.z)
      maxSize.z = vert.pos.z;

    if (vert.pos.x < minSize.x)
      minSize.x = vert.pos.x;
    if (vert.pos.y < minSize.y)
      minSize.y = vert.pos.y;
    if (vert.pos.z < minSize.z)
      minSize.z = vert.pos.z;

    m.vertices.push_back(vert);
    vertices.push_back(vert);
  }

  for (unsigned i = 0; i < mesh->mNumFaces; ++i)
  {
    const auto face = mesh->mFaces[i];
    if (face.mNumIndices < 3)
      continue;
    m.indices.push_back(face.mIndices[0]);
    m.indices.push_back(face.mIndices[1]);
    m.indices.push_back(face.mIndices[2]);
  }
  

  glm::vec3 center = (maxSize + minSize) / 2.0f;
  float scale = abs(minSize.x - maxSize.x);

  glm::mat4 matrix(1.0f);
  matrix = glm::scale(matrix, glm::vec3(1.0f / scale));
  matrix = translate(matrix, glm::vec3(-center.x, -center.y, -center.z));

  float u, v;

  //Cylindrical 
  for (auto& preTransformVert : vertices)
  {
    glm::vec4 vert = matrix * glm::vec4(preTransformVert.pos, 1.0f);
    glm::vec4 newMin = matrix * glm::vec4(minSize, 1.0f);
    glm::vec4 newMax = matrix * glm::vec4(maxSize, 1.0f);

    //divide by 360 degrees   
    float theta = atan2(vert.z, vert.x); //if u is negative add 2PI
    u = (theta + PI) / (2.0f * PI);

    //y not z cause its y up not z up
    v = 1 - ((vert.y - newMin.y) / (newMax.y - newMin.y));
    m.uvCylindrical.push_back({u, v});
    if (u > 1.0f || u < 0.0f || v > 1.0f || v < 0.0f)
      std::cout << "u or v is out of range!" << std::endl;
  }

  //Spherical
  for (auto& preTransformVert : vertices)
  {
    glm::vec4 vert = matrix * glm::vec4(preTransformVert.pos, 1.0f);

    float theta = atan2(-vert.z, vert.x);
    u = (theta + PI) / (2.0f * PI);

    v = acosf(vert.y) / PI; // / r); r = 1

    m.uvSpherical.push_back({u, v});
    if (u > 1.0f || u < 0.0f || v > 1.0f || v < 0.0f)
      std::cout << "u or v is out of range!" << std::endl;
  }

  for (auto & preTransformVert : vertices)
  {
    glm::vec4 vert = matrix * glm::vec4(preTransformVert.pos, 1.0f);
    //0 = x, 1 = y, 2 = x
    glm::vec2 uv = uv_calc(vert);
    m.uvPlanar.push_back(uv);
  }



  /*
  std::vector<vec3> tangents;
  std::vector<vec3> bitangents;

  tangents.reserve(vertices.size());
  bitangents.reserve(vertices.size());

  for (int i = 0; i < vertices.size(); i += 3) {

    // Shortcuts for vertices
    glm::vec3 & v0 = vertices[i + 0].pos;
    glm::vec3 & v1 = vertices[i + 1].pos;
    glm::vec3 & v2 = vertices[i + 2].pos;

    // Shortcuts for UVs
    glm::vec2 & uv0 = m.uvSpherical[i + 0];
    glm::vec2 & uv1 = m.uvSpherical[i + 1];
    glm::vec2 & uv2 = m.uvSpherical[i + 2];

    // Edges of the triangle : position delta
    glm::vec3 deltaPos1 = v1 - v0;
    glm::vec3 deltaPos2 = v2 - v0;

    // UV delta
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

    // Set the same tangent for all three vertices of the triangle.
        // They will be merged later, in vboindexer.cpp
    tangents.push_back(tangent);
    tangents.push_back(tangent);
    tangents.push_back(tangent);

    // Same thing for bitangents
    bitangents.push_back(bitangent);
    bitangents.push_back(bitangent);
    bitangents.push_back(bitangent);

  }
  */

  /*
  // calculate tangent and bitangent manually because assimp is failing
  //after UV coords are calculated
  std::vector<std::vector<vec3>> tangents(m.indices.size());
  std::vector<std::vector<vec3>> bitangents(m.indices.size());

  //calculate face's bitanget/tangent



  for (unsigned i = 0; i < m.indices.size() - 2; i+= 3)
  {
    int index1 = m.indices[i];
    int index2 = m.indices[i + 1];
    int index3 = m.indices[i + 2];

    vec3 pos1 = m.vertices[index1].pos;
    vec3 pos2 = m.vertices[index2].pos;
    vec3 pos3 = m.vertices[index3].pos;

    vec2 uv1 = m.uvPlanar[index1];
    vec2 uv2 = m.uvPlanar[index2];
    vec2 uv3 = m.uvPlanar[index3];

    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    vec3 tangent;
    vec3 bitangent;

    tangent.x= f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangents[index1].push_back((tangent));
    tangents[index2].push_back((tangent));
    tangents[index3].push_back((tangent));

    //m.vertices[i + 1].tangent = normalize(m.vertices[i].tangent);
    //m.vertices[i + 2].tangent = normalize(m.vertices[i].tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangents[index1].push_back((bitangent));
    bitangents[index2].push_back((bitangent));
    bitangents[index3].push_back((bitangent));
    //m.vertices[index1].tangent = tangent;
    //m.vertices[index2].tangent = tangent;
    //m.vertices[index3].tangent = tangent;
    //m.vertices[index1].bitangent = bitangent;
    //m.vertices[index2].bitangent = bitangent;
    //m.vertices[index3].bitangent = bitangent;
                       
  }


  for (unsigned i = 0; i < m.indices.size(); ++i)
  {
    int index = m.indices[i]; //gets the vertex from the face
    
    vec3 tangent = vec3(0);
    vec3 bitangent = vec3(0);

    for(unsigned j = 0; j < tangents[i].size(); ++j)
    {
      tangent += tangents[i][j];
      bitangent += bitangents[i][j];
    }
    m.vertices[index].tangent = tangent;// / float(tangents[index].size()));
    m.vertices[index].bitangent = bitangent;// normalize(bitangent / float(bitangents[index].size()));

  }
  */

}

void ObjectReader::load(const std::string& filename) noexcept
{
  Model mesh = load_model("models/" + filename);

  /*
    So here is where we ran unto a WONDERFUL issue with the editor/sizing models where the walls would not easily line up
   this is fixed by in Maya creating a plane that represents the 1x1(possibly 1x1x1) unit square the mesh will be scaled down too
   this is to allow things such as uncentered walls and corners to line up properly. the debug is not added to the actual render but
   is used in the 1x1 calculation. We decided the easiest way is solving this with the DEBUG_PLANE. Think of it as a meter stick
   used to measure and scale walls properly. Its only needed for room walls and things that need pixel perfect matching aka walls,
   and things that are on the grid that need to match other things on the grid. - Coleman
 */

  //if (mesh.name != std::string("DEBUG_PLANE"))
  //  info.meshes = (std::make_pair(mesh.get_vao(), mesh.indices.size()));

  //generate model to world transform
  //gets the min and max sizes of each

  
  for(auto& vert: mesh.vertices)
  {
    if (vert.pos.x > mesh.maxSize.x)
      mesh.maxSize.x = vert.pos.x;
    if (vert.pos.y > mesh.maxSize.y)
      mesh.maxSize.y = vert.pos.y;
    if (vert.pos.z > mesh.maxSize.z)
      mesh.maxSize.z = vert.pos.z;

    if (vert.pos.x < mesh.minSize.x)
      mesh.minSize.x = vert.pos.x;
    if (vert.pos.y < mesh.minSize.y)
      mesh.minSize.y = vert.pos.y;
    if (vert.pos.z < mesh.minSize.z)
      mesh.minSize.z = vert.pos.z;
  }

  mesh.name = filename;
  glm::vec3 scale = abs(mesh.minSize - mesh.maxSize);

  mesh.halfExtents = scale / 2.0f;

  //scale the object by the largest axis
  float maxVal = std::max(scale.x, std::max(scale.y, scale.z));

  glm::vec3 center = (mesh.maxSize + mesh.minSize) / 2.0f;

  mesh.modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / maxVal));
  mesh.modelMatrix = translate(mesh.modelMatrix, glm::vec3(-center.x, -center.y, -center.z));
  mesh.setup_mesh();
  models.push_back(mesh);
}

void ObjectReader::loadMultiple(const std::string& filename)
{

  std::string file = load_file(filename);

  //append new line to end of file
  file.push_back('\n');
  std::vector < std::string> objects;

  //is -1 because the first filename does not have a 
  size_t findPos = 0;
  size_t lastPos = file.find_last_of('\n');
  do
  {
    unsigned i = 1;

    //while still filename
    while (file.size() > findPos + i + 1 && file[findPos + i] != '\n')
    {
      ++i;
    }

    //push back filename
    //only push back file if its greater than size of 3 (for suffix *.obj)
    if (i > 4)
      objects.push_back(file.substr(findPos, i));
    file.erase(findPos, i);

    findPos = file.find_first_of('\n');
    if (findPos != std::string::npos)
    {
      file.erase(findPos, 1);
    }
    else
    {
      break;
    }

    //if (file.size() == 0 || findPos == std::string::npos)
     // break;
  } while (findPos != std::string::npos && findPos != lastPos);

  //load objects into loader

  for (auto& objName : objects)
  {
    load(objName);
  }
  

}

Model& ObjectReader::GetObject(int objNum)
{
  return models[std::min<unsigned>(objNum, models.size() - 1)];
}
