#include <pch.h>
#include "Projection.h"


glm::mat4 cameraToWorld(const Camera& cam)
{
  /*
  glm::mat4 Mc(1.0f);
  glm::vec3 U = cam.right();
  glm::vec3 V = cam.up();
  glm::vec3 N = cam.back();
  glm::vec3 E = cam.eye();
  
  Mc[0] = glm::vec4(glm::vec3(U), 0);
  Mc[1] = glm::vec4(glm::vec3(V), 0);
  Mc[2] = glm::vec4(glm::vec3(N), 0);
  Mc[3] = glm::vec4(glm::vec3(E), 1);
  return Mc;
  */
  return inverse(glm::lookAt(cam.eye(), cam.eye() + -cam.back_vector, cam.up_vector));
}


glm::mat4 worldToCamera(const Camera& cam)
{
  return glm::lookAt(cam.eye(), cam.eye() + -cam.back_vector, cam.up_vector);
  //return inverse(cameraToWorld(cam));
}

glm::mat4 cameraToNDC(Camera& cam)
{
  /*
  glm::mat4 M(1.0f);
  //W,H,D
  glm::vec3 v = cam.viewportGeometry();
  float N = cam.nearDistance();
  float F = cam.farDistance();
  M[0] = glm::vec4((2.0f * v.z)/v.x, 0,0,0);
  M[1] = glm::vec4(0,(2.0f * v.z)/v.y, 0,0);
  M[2] = glm::vec4(0,0,
     (N + F) / (N - F) , 
     (N * 2 * F) / (N - F));
  M[3] = glm::vec4(0,0,-1,0); 
  */
  auto& render = pattern::get<Render>();
  auto& input =  pattern::get<InputManager>();

  return glm::perspective(cam.zoomScale, float(render.windowX) / float(render.windowY), render.nearPlane, render.farPlane);
  //return transpose(M);  //glm is column not row 

}


glm::mat4 cameraToWorldSkyBox(const Camera& cam)
{
  glm::mat4 Mc(1.0f);
  glm::vec3 U = cam.right();
  glm::vec3 V = cam.up();
  glm::vec3 N = cam.back();
  glm::vec3 E = cam.eye();


  Mc[3] = glm::vec4(glm::vec3(E), 1);
  return Mc;
}

glm::mat4 worldToCameraSkyBox(const Camera& cam)
{
  return inverse(cameraToWorldSkyBox(cam));
}

