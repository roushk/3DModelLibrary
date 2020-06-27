#include <pch.h>

#include "Wireframe.h"
#include "Object.h"

//#include <glm/glm.hpp>
//#include <glm/ext.hpp>

Wireframe::Wireframe(const Wireframe& rhs)
{
  verts = rhs.verts;
  faces = rhs.faces;
  color = rhs.color;
  BindVertsAndFaces();
}

//creates wireframe out of object
Wireframe::Wireframe(Model& obj)
{
  auto& mesh = obj;

  
  for(unsigned i = 0; i < mesh.indices.size() + 2; i += 3)
  {
    
    //from tri a,b,c to line segments a,b a,c b,c
    faces.push_back({ mesh.indices[0], mesh.indices[1] });
    faces.push_back({ mesh.indices[0], mesh.indices[2] });
    faces.push_back({ mesh.indices[1], mesh.indices[2] });
  }

  for (auto& vert : mesh.vertices)
  {
    glm::vec3 postVert = obj.modelMatrix * mesh.meshMatrix * glm::vec4(vert.pos, 1);
    verts.push_back(postVert);
  }



  BindVertsAndFaces();
}



void Wireframe::BindVertsAndFaces()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, VBO);
  glGenBuffers(1, &IBO);
  glBindVertexArray(VAO);

  //faces
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof glm::uvec2, faces.data(), GL_STATIC_DRAW);
  //2
  glEnableVertexAttribArray(0);

  //verts
  glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof glm::vec3, verts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  glBindVertexArray(0);
}

Wireframe::~Wireframe()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, VBO);
  glDeleteBuffers(1, &IBO);
}
