/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: WireframeShader.frag
Purpose: Simple shader for wireframe objects
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_3
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 6/30/18
End Header --------------------------------------------------------*/

#version 430

//simple pass through
in VS_OUT
{
    vec4 rasterColor;
} fs_in;

out vec4 color;

void main()
{
  color = fs_in.rasterColor;
}
