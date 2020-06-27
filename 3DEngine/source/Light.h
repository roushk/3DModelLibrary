/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: Light.h
Purpose: Light class header and implementation for the light types
Language: C++ MSVC
Platform: VS 141, OpenGL 4.3 compatabile device driver, Win10
Project: coleman.jonas_CS350_1
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/20/18
End Header --------------------------------------------------------*/


#ifndef LIGHT_H
#define LIGHT_H
#include <glm/glm.hpp>
#include <algorithm>
#include <string>
#include "Remap.h"


class SceneLighting;
using namespace glm;

enum LightType
{
  ltNone,
  ltPoint,
  ltDirectional,
  ltSpotlight
};

class Light;

/*create new light and set type with load and name and provide data*/

class LightData
{
public:
  vec4 ambient = vec4(0, 0, 0, 0);
  vec4 diffuse = vec4(0, 0, 0, 0);
  vec4 specular = vec4(0, 0, 0, 0);
  vec4 position = vec4(0, 0, 0, 0);  //spotlight and point
  vec4 direction = vec4(0, 0, 0, 0); //spotlight and directional
  int type = ltNone;
  LightData() {};

  //all lights need this


  float outerRadius = 0;  //spotlight only
  float innerRadius = 0;  //spotlight only
  float falloffValue = 0; //spotlight only
};

class Light : public LightData
{
public:  
  Light(std::string name_): LightData() , name(name_) {};
  void SetSpotlight(float innerRadius_, float outerRadius_,float falloffValue_,
    vec3 ambient_, vec3 diffuse_, vec3 specular_)
  {
    Reset();
    name = std::string("Light");// +std::to_string(++lightID);

    type = ltSpotlight;
    innerRadius = innerRadius_; outerRadius = outerRadius_;
    ambient = vec4(ambient_, 1.0f);
    diffuse = vec4(diffuse_, 1.0f);
    specular = vec4(specular_, 1.0f);
    falloffValue = falloffValue_;
  };
  void SetPointLight(vec3 ambient_, vec3 diffuse_, vec3 specular_)
  {
    Reset();
    name = std::string("Light");// + std::to_string(++lightID);
    type = ltPoint;
    ambient = vec4(ambient_, 1.0f);
    diffuse = vec4(diffuse_, 1.0f);
    specular = vec4(specular_, 1.0f);
  };

  void SetDirectionalLight(vec3 ambient_, vec3 diffuse_, vec3 specular_)
  {
    Reset();
    name = std::string("Light");// + std::to_string(++lightID);
    type = ltDirectional;
    ambient = vec4(ambient_, 1.0f);
    diffuse = vec4(diffuse_, 1.0f);
    specular = vec4(specular_, 1.0f);
  };

  void Update()
  {
    if(type == ltDirectional || type == ltSpotlight)
    {
      direction = -position;
    }
  };
  void Reset()
  {
    name.clear();
    type = ltNone; //direction = vec4(0, 0, 0, 0);
    //position = vec4(0, 0, 0, 0);
    outerRadius = 0;
    innerRadius = 0;
    ambient = vec4(0, 0, 0, 0);
    diffuse = vec4(0, 0, 0, 0);
    specular = vec4(0, 0, 0, 0);
  };


  bool on = true;

  std::string name;

  void setRadius(SceneLighting& lighting, int maxLights);
  void SetPositionAndScaleCalc(const vec4& pos, float scale_)
  {
    modelTransform = translate(vec3(pos)) * scale(glm::mat4(1.0f), vec3(scale_));
  }
  void SetPositionAndScaleView(const vec4& pos, float scale_)
  {
    modelTransformView = translate(vec3(pos)) * scale(glm::mat4(1.0f), vec3(scale_));
  }
  glm::mat4& getMatrix() { return modelTransform; };

  static int lightID;
  int lightNum;
  float radius;
  glm::mat4 modelTransform = glm::mat4(1.0f);
  glm::mat4 modelTransformView = glm::mat4(1.0f);

};



class SceneLighting
{
  public:

  
  std::vector<const char*> GetLightNames()
  {
    std::vector<const char*> names;
    for(auto& light : lights)
    {
      names.push_back(light.name.c_str());
    }
    return names;
  };
  std::vector<const char*> GetLightTypes()
  {
    std::vector<const char*> names;
    names.push_back("No Light");  //point, dir, spot
    names.push_back("Point Light");
    names.push_back("Directional Light");
    names.push_back("Spotlight");
    return names;
  };
  std::vector<const char*> GetLightingTypes()
  {
    std::vector<const char*> names;
    names.push_back("Reflective Environment Shading");
    names.push_back("Refractive Environment Shading");
    names.push_back("Frensel Approximation Environment Shading");
    names.push_back("Phong Shading Model + Frensel Approximation Environment Shading");
    names.push_back("Phong Shading Model + Reflection");
    //names.push_back("Phong Shading Model");
    //names.push_back("Phong Lighting Model");
    //names.push_back("Blinn Lighting");

    return names;
  };


  //global lighting
  //fog
  struct Global
  {
   
    //float att = min(1.0f/(G.AttParam.x + G.AttParam.y * dL + G.AttParam.z * dL * dL), 1.0f);
    //x = constant, y = linear, z = quadratic
    vec4 AttenParam = { 0.0f,0.0f,0.0f, 1.8f};  //c1 = x, c2 = y, c3 = z
    vec4 FogColor = { 0,0,0 , 1 };
    vec4 Iglobal = { 0,0,0 , 1 };
    float Kglobal = 0;

    float near = -1;
    float far = -1;

    vec3 KMaterialambient = { 0.0f,0.1f,0.0f};  //is same as Iglobal??
    vec3 KMaterialemissive = { 0.1f,0.0f,0.0f};

  }global;
  int maxLights = 8;
  int activeLights = 1;
  std::vector<Light> lights;
};


inline void Light::setRadius(SceneLighting& lighting, int maxLights)
{
  float constant = lighting.global.AttenParam.x;
  float linear = lighting.global.AttenParam.y;
  float quadratic = lighting.global.AttenParam.z;

   //float brightestValuePerLight = 256.0 / 5.0;
  //brightest value is that each light can only be as bright as 1 / maxLights of the scene
  //assuming that 256 is the most light we can accumulate in the scene (1-255 bytes)
  float brightestValuePerLight = (1.0f / maxLights) * 256.0f;

  float lightMax = std::fmaxf(std::fmaxf(diffuse.r, diffuse.g), diffuse.b);
  float inside2 =  (constant - (brightestValuePerLight * lightMax));
  double inside = std::sqrtf(std::abs(linear * linear - 4.0f * quadratic * inside2));
  //radius = (-linear + inside) / (2 * quadratic);
  radius = 2 * (-linear + inside) / (quadratic);

}

#endif

