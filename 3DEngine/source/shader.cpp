#include "pch.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <GL/glew.h>

#include "shader.hpp"
using namespace std;

GLuint LoadComputeShader(const char* compute_file_path)
{
  // Create the shaders
  GLuint ComputeShaderID = glCreateShader(GL_COMPUTE_SHADER);

  // Read the Compute Shader code from the file
  std::string ComputeShaderCode;
  std::ifstream ComputeShaderStream(compute_file_path, std::ios::in);
  if (ComputeShaderStream.is_open())
  {
    std::string Line = "";
    while (getline(ComputeShaderStream, Line))
      ComputeShaderCode += "\n" + Line;
    ComputeShaderStream.close();
  }
  else
  {
    printf("Impossible to open %s. Are you in the right directory ?\n", compute_file_path);
    getchar();
    return 0;
  }

  GLint Result = GL_FALSE;
  int InfoLogLength;


  // Compile Compute Shader
  printf("Compiling shader : %s\n", compute_file_path);
  char const * ComputeSourcePointer = ComputeShaderCode.c_str();
  glShaderSource(ComputeShaderID, 1, &ComputeSourcePointer, NULL);
  glCompileShader(ComputeShaderID);

  // Check Compute Shader
  glGetShaderiv(ComputeShaderID, GL_COMPILE_STATUS, &Result);
  glGetShaderiv(ComputeShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ComputeShaderErrorMessage(InfoLogLength + 1);
    glGetShaderInfoLog(ComputeShaderID, InfoLogLength, NULL, &ComputeShaderErrorMessage[0]);
    printf("%s\n", &ComputeShaderErrorMessage[0]);
  }



  // Link the program
  printf("Linking program\n");
  GLuint ProgramID = glCreateProgram();
  glAttachShader(ProgramID, ComputeShaderID);
  glLinkProgram(ProgramID);

  // Check the program
  glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
  glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  if (InfoLogLength > 0) {
    std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    printf("%s\n", &ProgramErrorMessage[0]);
  }

  //glDetachShader(ProgramID, ComputeShaderID);
  //glDetachShader(ProgramID, FragmentShaderID);

  GLenum err;
  ///////////////////////////////////////////////////
  while ((err = glGetError()) != GL_NO_ERROR)
  {
    std::cout << "There was an Error: " << err << std::endl;
  }
  ///////////////////////////////////////////////////

  glDeleteShader(ComputeShaderID);

  return ProgramID;
}

GLuint LoadShaders(const char * Vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(Vertex_file_path, std::ios::in);
	if(VertexShaderStream.is_open())
  {
		std::string Line = "";
		while(getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
  else
  {
		printf("Impossible to open %s. Are you in the right directory ?\n", Vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if(FragmentShaderStream.is_open())
  {
		std::string Line = "";
		while(getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}
  else 
  {
    printf("Impossible to open %s. Are you in the right directory ?\n", fragment_file_path);
    getchar();
    return 0;
  }


	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", Vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}


	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}


	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	//glDetachShader(ProgramID, ComputeShaderID);
	//glDetachShader(ProgramID, FragmentShaderID);
	
  GLenum err;
  ///////////////////////////////////////////////////
  while ((err = glGetError()) != GL_NO_ERROR)
  {
    std::cout << "There was an Error: " << err << std::endl;
  }
  ///////////////////////////////////////////////////

  glDeleteShader(VertexShaderID);
  glDeleteShader(FragmentShaderID);

	return ProgramID;
}

