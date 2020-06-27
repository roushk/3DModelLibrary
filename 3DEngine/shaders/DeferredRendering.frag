/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: PhongShading.frag
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_2
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/30/18
End Header --------------------------------------------------------*/

#version 430

uniform sampler2D gPositionMap; //position map.a = emi
uniform sampler2D gNormalMap;
uniform sampler2D gDiffuseMap;
uniform sampler2D gSpecularMap;
uniform sampler2D gTangentMap;
uniform sampler2D gBitangentMap;

uniform sampler2D shadowMap;
uniform sampler2D blurShadowMapHorizontal;
uniform sampler2D blurShadowMapVertical;
uniform sampler2D momentShadowMap;

uniform sampler2D normalMap;
uniform sampler2D heightMap;

uniform sampler2D SSAOBlurMapHorizontal;
uniform sampler2D SSAOBlurMapVertical;
uniform sampler2D SSAOMap;

uniform uint debugTexture;
in VS_OUT
{
  vec2 texCoords;
} fs_in;

out vec4 fragColor;

void main()
{
  vec3 viewPos = texture(gPositionMap, fs_in.texCoords).xyz;
  vec3 normal = texture(gNormalMap, fs_in.texCoords).xyz;
  vec3 diffuse = texture(gDiffuseMap, fs_in.texCoords).xyz;
  vec3 specular = texture(gSpecularMap, fs_in.texCoords).xyz;

  vec3 shadow = vec3(texture(shadowMap, fs_in.texCoords).r);
  vec3 blurShadowH = texture(blurShadowMapHorizontal, fs_in.texCoords).rgb;
  vec3 blurShadowV = texture(blurShadowMapVertical, fs_in.texCoords).rgb;
  vec3 momentShadow = vec3(texture(momentShadowMap, fs_in.texCoords).rgb);
 
  vec3 tangent = vec3(texture(gTangentMap, fs_in.texCoords).rgb);
  vec3 bitangent = vec3(texture(gBitangentMap, fs_in.texCoords).rgb);

  vec3 height = vec3(texture(heightMap, fs_in.texCoords).rgb);
  vec3 normalmapped = vec3(texture(normalMap, fs_in.texCoords).rgb);

  vec3 SSAOBlurH = vec3(texture(SSAOBlurMapHorizontal, fs_in.texCoords).r);
  vec3 SSAOBlurV = vec3(texture(SSAOBlurMapVertical, fs_in.texCoords).r);
  vec3 SSAO = vec3(texture(SSAOMap, fs_in.texCoords).r);
  //normal = normalize(normal);
  vec3 debugColor = vec3(0,0,0);

  if(debugTexture == 0)
  {
    debugColor = viewPos;
  }
  else if(debugTexture == 1)
  {
    debugColor = normal;
  }
  else if(debugTexture == 2)
  {
    debugColor = diffuse;
  }
  else if(debugTexture == 3)
  {
    debugColor = specular;
  }
  else if(debugTexture == 4)
  {
    debugColor = shadow;
  }
  else if(debugTexture == 5)
  {
    debugColor = blurShadowH;
  }
  else if(debugTexture == 6)
  {
    debugColor = blurShadowV;
  }
  else if(debugTexture == 7)
  {
    debugColor = SSAO;
  }
  else if(debugTexture == 8)
  {
    debugColor = SSAOBlurH;
  }
  else if(debugTexture == 9)
  {
    debugColor = SSAOBlurV;
  }
  else if(debugTexture == 10)
  {
    debugColor = tangent;
  }


    

  fragColor = vec4(debugColor, 1.0f);
}