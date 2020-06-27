#include <pch.h>
#define PI 3.14159265359f
#define _CRT_SECURE_NO_WARNINGS

// Include standard headers
//#include <stdio.h>
//#include <stdlib.h>


// Include GLEW
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "Projection.h"
#include "SOIL/SOIL.h"
#include "shader.hpp"

#include "Render.h"
#include "Wireframe.h"
#include "singleton.h"
#include <minwindef.h>
#include "GUI.h"
#include "singleton.h"
#include "rgbe.h"
#include "loadfile.h"


Render::Render()
{
  InitRender();
  lightDatas.resize(8);
  //mvTransform = glm::mat4(1.0);
  //vTransform = glm::mat4(1.0);
}

Render::~Render()
{
  // Cleanup VBO
  glDeleteBuffers(2, uboHandle);

  //glDeleteVertexArrays(1, &VertexArrayID);
  glDeleteProgram(programIDs[0]);
  glDeleteProgram(programIDs[1]);
  glDeleteProgram(programIDs[2]);
  glDeleteProgram(programIDs[3]);
  glDeleteProgram(programIDs[4]);
  glDeleteProgram(programIDs[5]);

  glDeleteTextures(1, &specularMaterialID);
  glDeleteTextures(1, &diffuseMaterialID);

  glDeleteBuffers(6, FrameBuffers);
  glDeleteTextures(6, RenderedTextures);
  glDeleteBuffers(6, FBODepthBuffers);

  glDeleteBuffers(1, &Gbuffer);
  glDeleteTextures(7, GBufferTexture);
  glDeleteBuffers(1, &GBufferDepthBuffer);


  // Close the SDL window
  
  SDL_DestroyWindow(gWindow);
}

//also loads specular map
void Render::GenGBuffer()
{
  glGenFramebuffers(1, &Gbuffer);
  glGenTextures(7, GBufferTexture);
  glGenRenderbuffers(1, &GBufferDepthBuffer);


  specularMaterialID = LoadTextureInto(std::string("materials/DiffuseMap.png"));


  for(auto& texture: textureMaps)
  {
    texture.diffuseID = LoadTextureInto(texture.diffuse);
    texture.normalID = LoadTextureInto(texture.normal);
    texture.heightID = LoadTextureInto(texture.height);
  }
}

std::string Render::GetSetting()
{
  return renderSetting;
}

void Render::SetModelOffset(float x, float y, float z, float scale_)
{
  modelTransform = translate(vec3(x, y, z)) * scale(glm::mat4(1.0f),vec3(scale_));
}

void Render::SetModelOffset(vec4 pos, float scale_)
{
  //modelTransform = translate(scale(glm::mat4(1.0f), vec3(scale_)), vec3(pos));
  modelTransform = translate(vec3(pos)) * scale(glm::mat4(1.0f), vec3(scale_));
}

//because static lights arent moving we dont want to recalculate its model transform
//every single frame, slows down alot
void Render::SetModelOffset(const Light& light)
{
  //modelTransform = translate(scale(glm::mat4(1.0f), vec3(scale_)), vec3(pos));
  modelTransform = light.modelTransform;
}
void Render::SetModelOffsetView(const Light& light)
{
  //modelTransform = translate(scale(glm::mat4(1.0f), vec3(scale_)), vec3(pos));
  modelTransform = light.modelTransformView;
}

void Render::SetModelOffset(vec3 pos, float scale_)
{
  modelTransform = translate(pos) * scale(glm::mat4(1.0f), vec3(scale_));
}

void Render::LoadMaterial()
{
  diffuseMaterialID = textureMaps[pattern::get<GUI>().currentTextureMaps].diffuseID;
}

void Render::LoadSkybox(std::vector<std::string>& skyboxNames)
{
  for (unsigned i = 0; i < skyboxNames.size(); ++i)
  {
    if (i == 2 || i == 3) //y+ or y-
      skyboxTextureID[i] = SOIL_load_OGL_texture(skyboxNames[i].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                                 SOIL_FLAG_TEXTURE_REPEATS);
    else
      skyboxTextureID[i] = SOIL_load_OGL_texture(skyboxNames[i].c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
                                                 SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y);
    glBindTexture(GL_TEXTURE_2D, skyboxTextureID[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
}


void Render::LoadSkydome()
{

  std::string filename = "skydomeFilenames.txt";
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
  for (unsigned i = 0; i < objects.size(); i += 2)
  {

    skydomeID[i / 2] = LoadHDRimage(objects[i]);
    skydomeIDIRR[i / 2] = LoadHDRimage(objects[i + 1]);

  }

}

void Render::BindSkydome()
{
  glUseProgram(programID);

  //Loads skydome
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, skydomeID[0]);
  glUniform1i(glGetUniformLocation(programID, "skydomeTexture"), 7);
  glBindSampler(GL_TEXTURE7, glGetUniformLocation(programID, "skydomeTexture"));

  //Loads skydome irradiance map
  glActiveTexture(GL_TEXTURE8);
  glBindTexture(GL_TEXTURE_2D, skydomeIDIRR[0]);
  glUniform1i(glGetUniformLocation(programID, "skydomeIRR"), 8);
  glBindSampler(GL_TEXTURE8, glGetUniformLocation(programID, "skydomeIRR"));

}

void Render::GenFrameBuffers()
{
  glGenFramebuffers(6, FrameBuffers);
  glGenTextures(6, RenderedTextures);
  glGenRenderbuffers(6, FBODepthBuffers);

  glGenFramebuffers(1, shadowFBO);
  glGenTextures(1, shadowTexture);

  //glGenFramebuffers(1, blurShadowFBO);
  glGenTextures(2, blurShadowTexture);
  

  glGenFramebuffers(1, SSAOFBO);
  glGenTextures(2, SSAOBlurTexture);
  glGenTextures(1, SSAOTexture);

}


void Render::resize(int w, int h)
{
  height = h;
  SDL_SetWindowSize(gWindow, w, height);
  glViewport(0, 0, w, height);
  aspect = float(w) / float(height);
  currentCamera = Camera(vec4{ 0, 0, 5, 0 }, vec4{ 0, 0, -1, 0 }, vec4{ 0,1,0,1 }, PI / 2.0f, aspect, nearPlane, farPlane);
}


void Render::BindShadowTextures()
{
  glActiveTexture(GL_TEXTURE13);
  glBindTexture(GL_TEXTURE_2D, shadowTexture[0]);
  glUniform1i(glGetUniformLocation(programID, "shadowMap"), 13);
  glBindSampler(GL_TEXTURE13, glGetUniformLocation(programID, "shadowMap"));
}


void Render::BindAndCreateShadowBuffers()
{
  int width = height * aspect;

  glActiveTexture(GL_TEXTURE13);
  glBindTexture(GL_TEXTURE_2D, shadowTexture[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width * shadowScale,
    height * shadowScale, 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[0]);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowTexture[0], 0);
  //glDrawBuffers(1, shadowBuffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Shadow Map Bind and Create Failed" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Render::BindShadowBuffer()
{
  //bind frame buffer i 
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO[0]);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Shadow Map buffer bind Failed" << std::endl;
  }

  glViewport(0, 0, height * aspect* shadowScale, height* shadowScale);
  
  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.5, 0.5, 0.5, 1);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //draw scene
}


void Render::BindAndCreateSSAOBlurBuffers()
{
  int width = height * aspect;

  glBindTexture(GL_TEXTURE_2D, SSAOBlurTexture[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width * shadowScale,
    height* shadowScale, 0, GL_RED, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  //single depth 32 bit float
  glBindTexture(GL_TEXTURE_2D, SSAOBlurTexture[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width * shadowScale,
    height* shadowScale, 0, GL_RED, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}



void Render::BindAndCreateBlurShadowBuffers()
{
  int width = height * aspect;

  glBindTexture(GL_TEXTURE_2D, blurShadowTexture[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width * shadowScale,
    height* shadowScale, 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


  glBindTexture(GL_TEXTURE_2D, blurShadowTexture[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width * shadowScale,
    height* shadowScale, 0, GL_RGBA, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void Render::CreateBlurShadowData()
{
  int w = blurValue;
  float s = w / 2;

  float totalWeight = 0;

  //goes from -w to w indexing i + w so that index range is 0 = > 2w + 1
  for (int i = -w; i <= w; ++i)
  {
    weights[i + w] = glm::exp((-0.5f) * (i / s) * (i / s));
    totalWeight += weights[i + w];
  }


  //normalizes weights
  for (unsigned i = 0; i < blurValue * 2 + 1; ++i)
  {
    weights[i] /= totalWeight;
  }
}

void Render::BlurShadowLoadFinalMap()
{
  glActiveTexture(GL_TEXTURE15);
  glBindTexture(GL_TEXTURE_2D, blurShadowTexture[1]);
  glUniform1i(glGetUniformLocation(programID, "blurShadowMap"), 15);
  glBindSampler(GL_TEXTURE15, glGetUniformLocation(programID, "blurShadowMap"));

}

void Render::HammersleyLoadData()
{
  glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "HammersleyBlock"), bpHammersley);

  glBindBufferBase(GL_UNIFORM_BUFFER, bpHammersley, HammersleyUBOHandle[0]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(hammersleyBlock), &hammersleyBlock, GL_STATIC_DRAW);


}

void Render::BlurShadowLoadData()
{
  glUniformBlockBinding(programID, glGetUniformBlockIndex(programID, "blurKernel"), bpShadowblur);

  glBindBufferBase(GL_UNIFORM_BUFFER, bpShadowblur, shadowBlurUBOHandle[0]);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(weights), weights, GL_STATIC_DRAW);


}

void Render::BlurShadowLoadHorizontal()
{

  //same as the shadow texture
  glBindImageTexture(0, shadowTexture[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
  glUniform1i(glGetUniformLocation(programID, "src"), 0);

  glBindImageTexture(1, blurShadowTexture[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glUniform1i(glGetUniformLocation(programID, "dst"), 1);
}

void Render::BlurShadowLoadVertical()
{

  //same as the shadow texture
  glBindImageTexture(0, blurShadowTexture[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
  glUniform1i(glGetUniformLocation(programID, "src"), 0);

  glBindImageTexture(1, blurShadowTexture[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glUniform1i(glGetUniformLocation(programID, "dst"), 1);
}


void Render::SSAOBlurLoadHorizontal()
{
  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
    glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));


  //same as the shadow texture depth map
  glBindImageTexture(0, GBufferTexture[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  glUniform1i(glGetUniformLocation(programID, "positionMap"), 0);

  //position map
  glBindImageTexture(1, GBufferTexture[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  glUniform1i(glGetUniformLocation(programID, "normalMap"), 1);

  //dest is SSAOBlurTexture[0]
  glBindImageTexture(2, SSAOBlurTexture[0], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
  glUniform1i(glGetUniformLocation(programID, "dst"), 2);

  glBindImageTexture(3, SSAOTexture[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
  glUniform1i(glGetUniformLocation(programID, "SSAOMap"), 3);
}

void Render::SSAOBlurLoadVertical()
{
  //same as the shadow texture
  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
    glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));

  glBindImageTexture(0, GBufferTexture[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  glUniform1i(glGetUniformLocation(programID, "positionMap"), 0);

  //position map
  glBindImageTexture(1, GBufferTexture[1], 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA16F);
  glUniform1i(glGetUniformLocation(programID, "normalMap"), 1);

  //dest is SSAOBlurTexture[1]
  glBindImageTexture(2, SSAOBlurTexture[1], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
  glUniform1i(glGetUniformLocation(programID, "dst"), 2);

  glBindImageTexture(3, SSAOBlurTexture[0], 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
  glUniform1i(glGetUniformLocation(programID, "SSAOMap"), 3);
}


void Render::BlurShadowLoadDebug()
{

  glActiveTexture(GL_TEXTURE14);
  glBindTexture(GL_TEXTURE_2D, blurShadowTexture[0]);
  glUniform1i(glGetUniformLocation(programID, "blurShadowMapHorizontal"), 14);
  glBindSampler(GL_TEXTURE14, glGetUniformLocation(programID, "blurShadowMapHorizontal"));


  glActiveTexture(GL_TEXTURE15);
  glBindTexture(GL_TEXTURE_2D, blurShadowTexture[1]);
  glUniform1i(glGetUniformLocation(programID, "blurShadowMapVertical"), 15);
  glBindSampler(GL_TEXTURE15, glGetUniformLocation(programID, "blurShadowMapVertical"));

}

void Render::SSAOBlurLoadDebug()
{

  glActiveTexture(GL_TEXTURE16);
  glBindTexture(GL_TEXTURE_2D, SSAOBlurTexture[0]);
  glUniform1i(glGetUniformLocation(programID, "SSAOBlurMapHorizontal"), 16);
  glBindSampler(GL_TEXTURE16, glGetUniformLocation(programID, "SSAOBlurMapHorizontal"));


  glActiveTexture(GL_TEXTURE17);
  glBindTexture(GL_TEXTURE_2D, SSAOBlurTexture[1]);
  glUniform1i(glGetUniformLocation(programID, "SSAOBlurMapVertical"), 17);
  glBindSampler(GL_TEXTURE17, glGetUniformLocation(programID, "SSAOBlurMapVertical"));

}

void Render::SSAOCreateFBO()
{

  int width = height * aspect;

  glActiveTexture(GL_TEXTURE18);
  glBindTexture(GL_TEXTURE_2D, SSAOTexture[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width * shadowScale,
    height * shadowScale, 0, GL_RED, GL_FLOAT, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO[0]);

  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, SSAOTexture[0], 0);
  glDrawBuffers(1, SSAOBuffers);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Shadow Map Bind and Create Failed" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Render::SSAOBindFBO()
{
 
  //bind frame buffer i 
  glBindFramebuffer(GL_FRAMEBUFFER, SSAOFBO[0]);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "SSAO buffer bind Failed" << std::endl;
  }

  glViewport(0, 0, height * aspect * shadowScale, height * shadowScale);

  glClearColor(1,1,1,1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.5, 0.5, 0.5, 1);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //draw scene

}

void Render::SSAOLoadDebug()
{
  glActiveTexture(GL_TEXTURE19);
  glBindTexture(GL_TEXTURE_2D, SSAOTexture[0]);
  glUniform1i(glGetUniformLocation(programID, "SSAOMap"), 19);
  glBindSampler(GL_TEXTURE19, glGetUniformLocation(programID, "SSAOMap"));

}

/*
void Render::BindBlurShadowBuffer()
{
  //bind frame buffer i 
  glBindFramebuffer(GL_FRAMEBUFFER, blurShadowFBO[0]);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Shadow Map buffer bind Failed" << std::endl;
  }

  glViewport(0, 0, height * aspect* shadowScale, height* shadowScale);

  glClearColor(1, 1, 1, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //draw scene
}
*/
void Render:: BindAndCreateGBuffers()
{
  GLuint numBuffers = 5;
  glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer);
  int width = height * aspect;

  //Per GBuffer View Pos Out
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[0]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GBufferTexture[0], 0);
  //End Per GBuffer

  //Per GBuffer NormalOut
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[1]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GBufferTexture[1], 0);

  //End Per GBuffer

  //Per GBuffer DiffuseOut
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[2]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GBufferTexture[2], 0);
  //End Per GBuffer

  //Per GBuffer SpecularOut
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[3]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GBufferTexture[3], 0);
  //End Per GBuffer

  //Per GBuffer tangent out
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[4]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GBufferTexture[4], 0);
  //End Per GBuffer


  glDrawBuffers(numBuffers, DrawGBuffers);

  //Bind depth buffer
  glBindRenderbuffer(GL_RENDERBUFFER, GBufferDepthBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, GBufferDepthBuffer);
  
  //check for completeness
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "GBuffer Bind and Create Failed" << std::endl;
  }
}

void Render::BindGBufferTextures()
{
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[0]);
  glUniform1i(glGetUniformLocation(programID, "gPositionMap"), 2);
  glBindSampler(GL_TEXTURE2, glGetUniformLocation(programID, "gPositionMap"));

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[1]);
  glUniform1i(glGetUniformLocation(programID, "gNormalMap"), 3);
  glBindSampler(GL_TEXTURE3, glGetUniformLocation(programID, "gNormalMap"));

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[2]);
  glUniform1i(glGetUniformLocation(programID, "gDiffuseMap"), 4);
  glBindSampler(GL_TEXTURE4, glGetUniformLocation(programID, "gDiffuseMap"));

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[3]);
  glUniform1i(glGetUniformLocation(programID, "gSpecularMap"), 5);
  glBindSampler(GL_TEXTURE5, glGetUniformLocation(programID, "gSpecularMap"));

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, GBufferTexture[4]);
  glUniform1i(glGetUniformLocation(programID, "gTangentMap"), 6);
  glBindSampler(GL_TEXTURE6, glGetUniformLocation(programID, "gTangentMap"));

  
}

void Render::BindGBuffer()
{
  //bind frame buffer i 
  glBindFramebuffer(GL_FRAMEBUFFER, Gbuffer);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "G buffer bind Failed" << std::endl;
  }

  glViewport(0, 0, height * aspect, height);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //draw scene
}

void Render::BindAndCreateFrameBuffers(int i)
{
  glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[i]);
  glActiveTexture(GL_TEXTURE8 + i);

  glBindTexture(GL_TEXTURE_2D, RenderedTextures[i]);
  int width = height * aspect;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, width, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glBindRenderbuffer(GL_RENDERBUFFER, FBODepthBuffers[i]);

  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, width);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER, FBODepthBuffers[i]);

  glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0, RenderedTextures[i], 0);

  DrawBuffers = {GL_COLOR_ATTACHMENT0};

  GLuint numBuffers = 1;
  glDrawBuffers(numBuffers, &DrawBuffers);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Frame buffer " << i << " Failed" << std::endl;
  }
}

void Render::BindFrameBufferToRenderTo(unsigned i)
{
  //bind frame buffer i 
  glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffers[i]);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
  {
    std::cout << "Frame buffer " << i << " Failed" << std::endl;
  }

  glViewport(0, 0, height * aspect, height * aspect);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //draw scene
}

void Render::BindFrameBufferTextures()
{
  //Loads X
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[0]);
  glUniform1i(glGetUniformLocation(programID, "positiveX"), 2);
  glBindSampler(GL_TEXTURE2, glGetUniformLocation(programID, "positiveX"));

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[1]);
  glUniform1i(glGetUniformLocation(programID, "negativeX"), 3);
  glBindSampler(GL_TEXTURE3, glGetUniformLocation(programID, "negativeX"));

  //Loads Y
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[2]);
  glUniform1i(glGetUniformLocation(programID, "positiveY"), 4);
  glBindSampler(GL_TEXTURE4, glGetUniformLocation(programID, "positiveY"));

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[3]);
  glUniform1i(glGetUniformLocation(programID, "negativeY"), 5);
  glBindSampler(GL_TEXTURE5, glGetUniformLocation(programID, "negativeY"));

  //Loads Z
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[4]);
  glUniform1i(glGetUniformLocation(programID, "positiveZ"), 6);
  glBindSampler(GL_TEXTURE6, glGetUniformLocation(programID, "positiveZ"));

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, RenderedTextures[5]);
  glUniform1i(glGetUniformLocation(programID, "negativeZ"), 7);
  glBindSampler(GL_TEXTURE7, glGetUniformLocation(programID, "negativeZ"));
}

void Render::BindDefaultFrameBuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, height * aspect, height);
}

void Render::BindSkybox()
{
  glUseProgram(programID);

  //Loads X
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[0]);
  glUniform1i(glGetUniformLocation(programID, "positiveX"), 2);
  glBindSampler(GL_TEXTURE2, glGetUniformLocation(programID, "positiveX"));

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[1]);
  glUniform1i(glGetUniformLocation(programID, "negativeX"), 3);
  glBindSampler(GL_TEXTURE3, glGetUniformLocation(programID, "negativeX"));

  //Loads Y
  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[2]);
  glUniform1i(glGetUniformLocation(programID, "positiveY"), 4);
  glBindSampler(GL_TEXTURE4, glGetUniformLocation(programID, "positiveY"));


  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[3]);
  glUniform1i(glGetUniformLocation(programID, "negativeY"), 5);
  glBindSampler(GL_TEXTURE5, glGetUniformLocation(programID, "negativeY"));


  //Loads Z
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[4]);
  glUniform1i(glGetUniformLocation(programID, "positiveZ"), 6);
  glBindSampler(GL_TEXTURE6, glGetUniformLocation(programID, "positiveZ"));


  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, skyboxTextureID[5]);
  glUniform1i(glGetUniformLocation(programID, "negativeZ"), 7);
  glBindSampler(GL_TEXTURE7, glGetUniformLocation(programID, "negativeZ"));
}

void Render::BindMaterials(SceneLighting& lighting)
{
  //glUseProgram(programID);

  //loads diffuse
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, diffuseMaterialID);
  glUniform1i(glGetUniformLocation(programID, "Kdiffuse"), 0);

  //loads specular
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, specularMaterialID);
  glUniform1i(glGetUniformLocation(programID, "Kspecular"), 1);

  //loads ambient and emissive from GUI
  glUniform3fv(glGetUniformLocation(programID, "Kambient"), 1,
               glm::value_ptr(lighting.global.KMaterialambient)); //works
  glUniform3fv(glGetUniformLocation(programID, "Kemissive"), 1,
               glm::value_ptr(lighting.global.KMaterialemissive)); //works
}

void Render::LoadModel(Wireframe& object)
{
  glUseProgram(programID);
  glBindVertexArray(object.VAO);
  glUniform4fv(glGetUniformLocation(programID, "color"), 1, glm::value_ptr(object.color));

}


void Render::LoadModel(Model& object)
{
  glUseProgram(programID);
  glBindVertexArray(object.get_vao());


  }

void Render::ClearScreen()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void Render::CreateShaders()
{
  GetError();
  //programIDs[ssBlinnLighting] = LoadShaders("shaders/BlinnLighting.vert", "shaders/BlinnLighting.frag");
  //programIDs[ssPhongLighting] = LoadShaders("shaders/PhongLighting.vert", "shaders/PhongLighting.frag");
  //programIDs[ssPhongShading] = LoadShaders("shaders/PhongShading.vert", "shaders/PhongShading.frag");
  programIDs[ssLightShader] = LoadShaders("shaders/LightShader.vert", "shaders/LightShader.frag");
  //programIDs[ssSkyboxShader] = LoadShaders("shaders/SkyboxShader.vert", "shaders/SkyboxShader.frag");
  //programIDs[ssPhongShadingAndReflection] = LoadShaders("shaders/PhongShadingAndReflection.vert",
  //                                                      "shaders/PhongShadingAndReflection.frag");
  //programIDs[ssReflectionMap] = LoadShaders("shaders/ReflectionMap.vert", "shaders/ReflectionMap.frag");
  //programIDs[ssRefractionMap] = LoadShaders("shaders/RefractionMap.vert", "shaders/RefractionMap.frag");
  //programIDs[ssFrenselApprox] = LoadShaders("shaders/Frensel.vert", "shaders/Frensel.frag");
  //programIDs[ssPhongShadingAndFrensel] = LoadShaders("shaders/PhongShadingAndFrensel.vert",
  //                                                   "shaders/PhongShadingAndFrensel.frag");
  programIDs[ssWireframe] = LoadShaders("shaders/WireframeShader.vert", "shaders/WireframeShader.frag");

  programIDs[ssDeferredGeometry] = LoadShaders("shaders/DeferredGeometry.vert", "shaders/DeferredGeometry.frag");
  programIDs[ssDeferredRendering] = LoadShaders("shaders/DeferredRendering.vert", "shaders/DeferredRendering.frag");

  programIDs[ssPhongShadingDeferred] = LoadShaders("shaders/DeferredRendering.vert", "shaders/PhongShadingDeferred.frag");
  programIDs[ssPhongShadingDeferredLightSphere] = LoadShaders("shaders/DeferredRenderingLightSphere.vert", "shaders/PhongShadingDeferredLightSphere.frag");
  programIDs[ssShadowShader] = LoadShaders("shaders/ShadowShader.vert", "shaders/ShadowShader.frag");
  programIDs[ssPhongShadingDeferredShadow] = LoadShaders("shaders/DeferredRendering.vert", "shaders/PhongShadingDeferredShadow.frag");
  
  programIDs[ssPhongShadingDeferredShadowMSM] = LoadShaders("shaders/DeferredRendering.vert", "shaders/PhongShadingDeferredShadowMSM.frag");
  programIDs[ssBRDFDeferredMSM] = LoadShaders("shaders/DeferredRendering.vert", "shaders/BRDFDeferredMSM.frag");
  programIDs[ssSkydome] = LoadShaders("shaders/SkyDome.vert", "shaders/SkyDome.frag");
  
  programIDs[ssComputeBlurHorizontal] = LoadComputeShader("shaders/ComputeBlurHorizontal.comp");
  programIDs[ssComputeBlurVertical] = LoadComputeShader("shaders/ComputeBlurVertical.comp");
  
  programIDs[ssSSAOBlurHorizontal] = LoadComputeShader("shaders/EdgeAwareBlurHorizontal.comp");
  programIDs[ssSSAOBlurVertical] = LoadComputeShader("shaders/EdgeAwareBlurVertical.comp");

  programIDs[ssSSAO] = LoadShaders("shaders/DeferredRendering.vert", "shaders/SSAO.frag");

  programID = programIDs[ssLightShader];
}


void Render::LoadMaxDepth()
{
  glUniform1f(glGetUniformLocation(programID, "max_depth"), max_depth);
  glUniform1f(glGetUniformLocation(programID, "scalarLevel"), scalarLevel);
}


void Render::BufferToneMapping()
{
  glUniform1f(glGetUniformLocation(programID, "exposure"), exposure);
  glUniform1f(glGetUniformLocation(programID, "contrast"), contrast);
}
void Render::LoadRoughness(float roughness)
{
  glUniform1f(glGetUniformLocation(programID, "materialAlpha"), roughness);
}
void Render::LoadDiffuseForLight(Light& light, float scale)
{
  glUniform3fv(glGetUniformLocation(programID, "diffuse"), 1, glm::value_ptr(scale * glm::normalize(light.diffuse)));
}
void Render::LoadDiffuseForLight(glm::vec4& light, float scale)
{
  glUniform3fv(glGetUniformLocation(programID, "diffuse"), 1, glm::value_ptr(scale * light));
}


void Render::SetCurrentShader(int shader)
{
  programID = programIDs[shader];
  currentShader = shader;
  glUseProgram(programID);
}

void Render::BindLightScene(SceneLighting& lighting)
{
  //****************************************//
  //load once

  //load global data

  GLuint uboIndex2 = glGetUniformBlockIndex(programID, "Global");
  if (uboIndex2 != GL_INVALID_INDEX)
  {
    GLint uboSize2;
    //get block size
    glGetActiveUniformBlockiv(programID, uboIndex2, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize2);
    const GLchar* names2[] = {
      //needed for all light types

      "Global.AttParam",
      "Global.FogColor",
      "Global.Iglobal",
      "Global.near",
      "Global.far",
      "Global.Kglobal",
      "Global.activeLights"
    };
    GLuint indices2[7];
    glGetUniformIndices(programID, 7, names2, indices2);

    GLint offset2[7];
    glGetActiveUniformsiv(programID, 7, indices2, GL_UNIFORM_OFFSET, offset2);

    glUniformBlockBinding(programID, uboIndex2, bpGlobal);

    lighting.global.near = nearPlane;
    lighting.global.far = farPlane;

    //allocate block size of entire array
    if (uboBuffer[1] == NULL)
      uboBuffer[1] = (GLubyte *)malloc(uboSize2);


    memcpy(uboBuffer[1] + offset2[0], glm::value_ptr(lighting.global.AttenParam), sizeof(vec4));
    memcpy(uboBuffer[1] + offset2[1], glm::value_ptr(lighting.global.FogColor), sizeof(vec4));
    memcpy(uboBuffer[1] + offset2[2], glm::value_ptr(lighting.global.Iglobal), sizeof(vec4));

    memcpy(uboBuffer[1] + offset2[3], &lighting.global.near, sizeof(float));
    memcpy(uboBuffer[1] + offset2[4], &lighting.global.far, sizeof(float));
    memcpy(uboBuffer[1] + offset2[5], &lighting.global.Kglobal, sizeof(float));

    memcpy(uboBuffer[1] + offset2[6], &lighting.activeLights, sizeof(int));

    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle[1]);
    glBufferData(GL_UNIFORM_BUFFER, uboSize2, uboBuffer[1],
                 GL_STATIC_DRAW);

    // Step 7. Bind the base of the buffer
    glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex2, uboHandle[1]);
  }


  
  //lights.data()
  //pushes light data onto individual arrays from light structs
  for (int i = 0; i < lighting.activeLights; ++i)
  {
    //LightData light = lighting.lights[i]; //intentional object slicing to just get data
    lightDatas[i] = lighting.lights[i];
  }
  /*
  LightData light; //intentional object slicing to just get data
  for (int i = lighting.activeLights; i < lighting.maxLights; ++i)
  {
    lights.push_back(light);
  }
  */
  
  //****************************************//
  //load array of lights Global, Material
  //get block index
  GLuint uboIndex = glGetUniformBlockIndex(programID, "LightBlock"); //uboIndex = block_index
  if (uboIndex != GL_INVALID_INDEX)
  {
    GLint uboSize; //is max size (aka 8 lights)
    //get block size
    glGetActiveUniformBlockiv(programID, uboIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &uboSize);

    glUniformBlockBinding(programID, uboIndex, bpLights);

    if (uboSize != sizeof(LightData) * lighting.maxLights)
      std::cout << "UBO and Light Data Struct size mismatch" << std::endl;

    int size = sizeof(LightData);

    memcpy(uboBuffer[0], lightDatas.data(), uboSize);

    glBindBuffer(GL_UNIFORM_BUFFER, uboHandle[0]);
    glBufferData(GL_UNIFORM_BUFFER, uboSize, uboBuffer[0], GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_UNIFORM_BUFFER, uboIndex, uboHandle[0]);
  }

  // Load camera
  //glm::vec3 cameraPos(0,0,-2);// = camera.eye();
}

void Render::CreateUBOBufferObjects()
{
  glGenBuffers(2, uboHandle);
  glGenBuffers(1, shadowBlurUBOHandle);
  glGenBuffers(1, HammersleyUBOHandle);
  uboBuffer[0] = (GLubyte*)malloc(sizeof(LightData) * 8);
}

void Render::CleanUpUBOBuffers(int activeLights)
{
  free(uboBuffer[1]);
  if (activeLights > 0)
    free(uboBuffer[0]);
}

void Render::SetCurrentDebugTexture(unsigned debugTextureNext)
{
  glUniform1ui(glGetUniformLocation(programID, "debugTexture"), debugTextureNext);

}

bool Render::InitRender()
{
  

  //initialise openGL window and context using SDL
  gWindow = nullptr;

  //Initialization flag
  creationSuccess = false;
  //Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cout << "SDL could not initialize the window! SDL_Error: %s\n" << SDL_GetError() << std::endl;
    creationSuccess = false;
    return creationSuccess;
  }
  else
  {
    std::string programName = "3DEngine";
    int screenWidth = 1024;
    int screenHeight = 768;

    //creates window and sets window flags
    gWindow = SDL_CreateWindow(programName.c_str(), SDL_WINDOWPOS_UNDEFINED,
      SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);


    if (gWindow == NULL)
    {
      std::cout << "Window could not be created! SDL_Error: %s\n" << SDL_GetError() << std::endl;

      creationSuccess = false;
      return creationSuccess;
    }

    gContext = SDL_GL_CreateContext(gWindow);

    //~~~~~~~~~~~~~~~~~~~~~//
    //SET OPENGL ATTRIBUTES//
    //~~~~~~~~~~~~~~~~~~~~~//

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    //sets OpenGL context version of the SDL window
    //set to OpenGL 4.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    //enableing double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);  //may need to be 16 or 32
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    //~~~~~~~~~~~~~~~~~~//
    //INITIALIZEING GLEW//
    //~~~~~~~~~~~~~~~~~~//

    //sets us to use newest versions of functions
    glewExperimental = GL_TRUE;

    glewInit();

    //sets refresh rate to the monitors refresh rate
    SDL_GL_SetSwapInterval(1);
    Update();
    GetError();
  }
}

void Render::CopyDepthBuffer()
{
  int width = height * aspect;
  glBindFramebuffer(GL_READ_FRAMEBUFFER, Gbuffer);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
  glBlitFramebuffer(
    0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
  //glBlitFramebuffer(
  //  0, 0, width, height, 0, 0, width, height, GL_STENCIL_BUFFER_BIT, GL_NEAREST);

}

void Render::BufferRefractionData()
{
  glUniform1f(glGetUniformLocation(programID, "highlightTightness"), highlightTightness);
  glUniform1f(glGetUniformLocation(programID, "transCoeff"), transmissionCoefficient);
}

void Render::LoadScreenSize()
{
  glUniform1i(glGetUniformLocation(programID, "height"), height);
  glUniform1i(glGetUniformLocation(programID, "width"), height * aspect);
}

void Render::LoadSSAOValues()
{
  glUniform1f(glGetUniformLocation(programID, "SSAOcontrast"), SSAOcontrast);
  glUniform1f(glGetUniformLocation(programID, "SSAOscale"), SSAOscale);
  glUniform1f(glGetUniformLocation(programID, "SSAOrange"), SSAOrange);

}
void Render::LoadSSAOBlurValues()
{
  glUniform1f(glGetUniformLocation(programID, "SSAOBlurScalar"), SSAOBlurScalar);
}

void Render::SSAOLoadBlur()
{
  glActiveTexture(GL_TEXTURE17);
  glBindTexture(GL_TEXTURE_2D, SSAOBlurTexture[1]);
  glUniform1i(glGetUniformLocation(programID, "SSAOBlurMap"), 17);
  glBindSampler(GL_TEXTURE17, glGetUniformLocation(programID, "SSAOBlurMap"));

  //toggle SSAO 
  glUniform1i(glGetUniformLocation(programID, "toggleSSAO"), pattern::get<GUI>().SSAO);

}


void Render::Update()
{
  //SDL_PollEvent();
  int width;
  SDL_GetWindowSize(gWindow, &width, &height);
  glViewport(0, 0, width, height);
  float oldAspect = aspect;
  aspect = float(width) / float(height);
  if (aspect != oldAspect)
  {
    cameraBase = Camera(vec4{0, 0, 5, 0}, vec4{0, 0, -1, 1}, vec4{0, 1, 0, 1}, PI / 2.0f, aspect, nearPlane, farPlane);
    cameraChanged = true;
  }

  Camera newCameras[6]
  {
    //same order as textures posx, negx, posy, negy, posz, negz
    //view vec os other dir b/c is lookAt
    //i have NO IDEA why SQRT2OVER2 works for the aspect ratio its not even 768/1024 its like 768/1086                      //orieng y up, x up
    Camera(vec4{objectPos, 0}, vec4{1, 0, 0, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient y -z
    Camera(vec4{objectPos, 0}, vec4{-1, 0, 0, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient y z
    Camera(vec4{objectPos, 0}, vec4{0, 1, 0, 1}, vec4{0, 0, -1, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient -z x
    Camera(vec4{objectPos, 0}, vec4{0, -1, 0, 1}, vec4{0, 0, 1, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient z x  //need to flip
    Camera(vec4{objectPos, 0}, vec4{0, 0, 1, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane), //orient y x
    Camera(vec4{objectPos, 0}, vec4{0, 0, -1, 1}, vec4{0, 1, 0, 1}, glm::radians(90.0f), SQRT2OVER2, nearPlane,
           farPlane) //orient y -x
  };
  for (unsigned i = 0; i < 6; ++i)
  {
    cameras[i] = newCameras[i];
  }

  
}


void Render::UpdateCamera(float dt)
{
  //vec3 pos{0, 1, 0}; //rotate around Y axis
  vec3 pos{ 0, 0, 1 }; //rotate around Z axis

  if (GUI::autoCameraRotation && updateCameraEyePosOnce == false)
  {
    eyePos = vec4(currentCamera.eye(), 1);
    updateCameraEyePosOnce = true;
  }

  if(GUI::rotateCamera)
  {
    eyePos = rotate(rotateRate * dt, pos) * eyePos;
  }

  //if(GUI::autoCameraRotation && updateCameraEyePosOnce)
  //{
  //}

  if (!GUI::autoCameraRotation && updateCameraEyePosOnce == true)
  {
    updateCameraEyePosOnce = false;
    currentCamera.eye_point = eyePos;
  }
  //lookAt = rotate(rotateRate * dt, pos) * lookAt;
  
  

  inverseCamRotate = rotate(rotateRate * dt, pos);
  
}

void Render::Draw(Wireframe& object)
{
  glUseProgram(programID);

  // Uniform transformation (vertex shader)
  
  if(cameraChanged == true)
  {
    projectionMatrix = cameraToNDC(currentCamera);
    viewMatrix = worldToCamera(currentCamera);
    cameraChanged = false;
  }

  if(GUI::autoCameraRotation)
  {
    auto& render = pattern::get<Render>();
    auto& input = pattern::get<InputManager>();

    projectionMatrix = glm::perspective(1.0f, float(render.windowX) / float(render.windowY), render.nearPlane, render.farPlane);
    vec3 backVec = lookAt - eyePos;
    viewMatrix = glm::lookAt(vec3(eyePos), vec3(eyePos) + -backVec, vec3(0,1,0));

  }
  //if (flipX == true)
  //  projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));

  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
    glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
  //already in world coords
  //glUniformMatrix4fv(glGetUniformLocation(programID, "modelTransform"), 1, GL_FALSE, glm::value_ptr(modelTransform));

  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  //2 not 3 because lines not faces
  glDrawElements(GL_LINES, object.faces.size() * 2, GL_UNSIGNED_INT, 0);
}

void Render::Draw(Model& object)
{
  glUseProgram(programID);


  
  glUniform3f(glGetUniformLocation(programID, "camera"),
    currentCamera.eye().x, currentCamera.eye().y, currentCamera.eye().z);
  // Uniform transformation (vertex shader)
  //if (cameraChanged == true)
  
  projectionMatrix = cameraToNDC(currentCamera);

  if (flipX == true)
    projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));

  viewMatrix = worldToCamera(currentCamera);
  //cameraChanged = false;

  if (GUI::autoCameraRotation)
  {
    auto& render = pattern::get<Render>();
    auto& input = pattern::get<InputManager>();

    projectionMatrix = glm::perspective(currentCamera.zoomScale, float(render.windowX) / float(render.windowY), render.nearPlane, render.farPlane);
    //vec3 backVec = normalize(lookAt - eyePos);
    viewMatrix = glm::lookAt(vec3(eyePos), vec3(lookAt), vec3(0, 1, 0));
    glUniform3f(glGetUniformLocation(programID, "camera"),
      eyePos.x, eyePos.y, eyePos.z);
  }

  glUniformMatrix4fv(glGetUniformLocation(programID, "shadowMatrix"), 1, GL_FALSE,
    glm::value_ptr(shadowMatrix));

  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
                     glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "modelTransform"), 1, GL_FALSE, glm::value_ptr(modelTransform));
  glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(object.modelMatrix));

  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glDrawElements(GL_TRIANGLES, object.indices.size() * 3, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle

}

//draws shadow to buffer
void Render::DrawShadow(const Model& object, const Light& light)
{
  glUseProgram(programID);

  glUniform3f(glGetUniformLocation(programID, "camera"),
    currentCamera.eye().x, currentCamera.eye().y, currentCamera.eye().z);
  // Uniform transformation (vertex shader)
  if (cameraChanged == true)
  {
    projectionMatrix = cameraToNDC(currentCamera);

    if (flipX == true)
      projectionMatrix = scale(projectionMatrix, vec3(-1, 1, 1));

    viewMatrix = worldToCamera(currentCamera);
    cameraChanged = false;
  }

  //projectionMatrix = cameraToNDC(lightCam);
  projectionMatrix = perspectiveFov<float>(radians(70.0f), height * aspect, height, nearPlane, farPlane);
  //projectionMatrix = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
  viewMatrix = glm::lookAt(glm::vec3(light.position), vec3(2.0f, 0, 0) , vec3(0, 1, 0));
  shadowMatrix = (glm::translate(vec3(0.5f)) * scale(vec3(0.5f))) * projectionMatrix * viewMatrix;
  //shadowMatrix = projectionMatrix * viewMatrix;
  glUniformMatrix4fv(glGetUniformLocation(programID, "shadowMatrix"), 1, GL_FALSE,
    glm::value_ptr(shadowMatrix));

  glUniformMatrix4fv(glGetUniformLocation(programID, "projectionMatrix"), 1, GL_FALSE,
    glm::value_ptr(projectionMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "viewMatrix"), 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(glGetUniformLocation(programID, "modelTransform"), 1, GL_FALSE, glm::value_ptr(modelTransform));
  glUniformMatrix4fv(glGetUniformLocation(programID, "modelMatrix"), 1, GL_FALSE, glm::value_ptr(object.modelMatrix));

  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glDrawElements(GL_TRIANGLES, object.indices.size() * 3, GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle

}

//returns textureID
GLuint Render::LoadHDRimage(std::string filename, bool irr)
{
  std::string inName = "IBL/" + filename;

  std::cout << "in: " << inName << std::endl;

  // Read the input file:  expect a *.hdr image
  int width, height;
  rgbe_header_info info;
  char errbuf[100] = { 0 };
  FILE* fp = fopen(inName.c_str(), "rb");
  int rc = RGBE_ReadHeader(fp, &width, &height, &info, errbuf);
  if (rc != RGBE_RETURN_SUCCESS) {
    printf("RGBE read error: %s\n", errbuf);
    exit(-1);
  }
  printf("%dx%d\n", width, height);

  std::vector<float> image(3 * width*height, 0.0);
  rc = RGBE_ReadPixels_RLE(fp, &image[0], width, height, errbuf);
  if (rc != RGBE_RETURN_SUCCESS) {
    printf("RGBE read error: %s\n", errbuf);
    exit(-1);
  }
  fclose(fp);

  // Gamma correct the image to linear color space.  Use gamma=2.2
  // if you have no specific gamma information. Skip this step if
  // you know image is already in linear space.
  /*
  // This is included to demonstrate the magic of OpenMP: This
  // pragma turns the following loop into a multi-threaded loop,
  // making use of all the cores your machine may have.
  //this gamma corrects
#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < width; i++) {
      int p = (j*width + i);
      for (int c = 0; c < 3; c++) {
        image[3 * p + c] *= pow(image[3 * p + c], 1.8);
      }
    }
  }
  */
  GLuint newTexture;
  glGenTextures(1, &newTexture);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, newTexture);


  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
    GL_FLOAT, image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, -100);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, 100);

  //glBindTexture(GL_TEXTURE_2D, 0);
  //glGenerateMipmap(GL_TEXTURE_2D);
  glGenerateTextureMipmap(newTexture);
  return newTexture;
}

void Render::EndDrawing()
{
  //glDisableVertexAttribArray(0);
  //glDisableVertexAttribArray(1);
  //glDisableVertexAttribArray(2);
  // Swap buffers

  SDL_GL_SwapWindow(gWindow);
}

void Render::ClearColor(vec4 color)
{
  glClearColor(color.x, color.y, color.z, color.w);
}

void Render::SetTitle(std::string object)
{
  std::string title = "Rendering: ";
  title += object;
  title += " Setting: ";
  title += renderSetting;

  SDL_SetWindowTitle(gWindow, title.c_str());
}

void Render::AddLight(Light light)
{
  lights.push_back(light);
}

void Render::ClearLights()
{
  lights.clear();
}

void Render::SetObjPositionForCamera(vec3 pos)
{
  objectPos = pos;
}

void Render::SetCurrentCamera(int cam)
{
  if (cam == 6)
    currentCamera = cameraBase;
  else
    currentCamera = cameras[cam];

  cameraChanged = true;
}

void Render::SetObjectShader(int shader)
{
  objectShader = shader;
}

void Render::LoadObjectShader()
{
  SetCurrentShader(objectShader);
}

GLuint Render::LoadTextureInto(std::string name)
{

  GLuint id;
  id = SOIL_load_OGL_texture(name.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
    SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  
  if (0 == id)
  {
    printf("SOIL loading error: '%s'\n", SOIL_last_result());
  }
  return id;
}

void Render::LoadNormalAndHeight()
{

  normalMap = textureMaps[pattern::get<GUI>().currentTextureMaps].normalID;
  heightMap = textureMaps[pattern::get<GUI>().currentTextureMaps].heightID;

}

/*
void Render::BindWidthAndHeight()
{
  
  glUniform1i(glGetUniformLocation(programID, "height"), height);
  glUniform1i(glGetUniformLocation(programID, "width"), height * aspect);


}
*/
void Render::BindNormalAndHeight()
{
  auto& gui = pattern::get<GUI>();
  glUniform1i(glGetUniformLocation(programID, "normalMapping"), gui.NormalMapping);
  glUniform1i(glGetUniformLocation(programID, "parallaxMapping"), gui.ParallaxMapping);
  glUniform1f(glGetUniformLocation(programID, "parallaxScale"), gui.ParallaxScale);

  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, normalMap);
  glUniform1i(glGetUniformLocation(programID, "normalMap"), 9);
  glBindSampler(GL_TEXTURE9, glGetUniformLocation(programID, "normalMap"));

  glActiveTexture(GL_TEXTURE10);
  glBindTexture(GL_TEXTURE_2D, heightMap);
  glUniform1i(glGetUniformLocation(programID, "heightMap"), 10);
  glBindSampler(GL_TEXTURE10, glGetUniformLocation(programID, "heightMap"));
}

void Render::HammersleyCreateData()
{
  
  hammersleyBlock.N = HammersleyConst; // N=20 ... 40 or whatever …
  int kk;
  int pos = 0;
  for (int k = 0; k < HammersleyConst; k++) 
  {
    kk = k;
    float u = 0.0f;
    for (float p = 0.5f; kk; p *= 0.5f, kk >>= 1)
    {
      if (kk & 1)
        u += p;
    }
    float v = (k + 0.5) / HammersleyConst;
    hammersleyBlock.hammersley[pos++] = u;
    hammersleyBlock.hammersley[pos++] = v;
  }
};


