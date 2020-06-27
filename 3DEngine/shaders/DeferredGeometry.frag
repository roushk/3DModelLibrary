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

in VS_OUT
{
  vec4 position;
  vec4 normal;
  vec2 texCoords;
  vec3 tangent;
  vec3 bitangent;
} fs_in;

//corrolates DIRECTLY to the COLOR_ATTACHMENT
layout (location = 0) out vec4 ViewPosOut;  //COLOR_ATTACH 0 + emissive x
layout (location = 1) out vec4 NormalOut;   //COLOR_ATTACH 1 + emissive y
layout (location = 2) out vec4 DiffuseOut;  //COLOR_ATTACH 2 + emissive z
layout (location = 3) out vec4 SpecularOut; //COLOR_ATTACH 3
layout (location = 4) out vec3 tangentOut;  //COLOR_ATTACH 4
layout (location = 5) out vec3 bitangentOut;  //COLOR_ATTACH 4
layout (location = 6) out vec3 DepthOut;    //DEPTH_ATTACH 1

uniform sampler2D Kdiffuse;
uniform sampler2D Kspecular;
uniform vec3 Kemissive; //Emissive: No texture, user specified value [RGB]
uniform vec3 Kambient;  //Ambient: No texture, user specified value [RGB]
uniform sampler2D normalMap;
uniform sampler2D heightMap;

uniform bool normalMapping;
uniform bool parallaxMapping;

uniform vec3 camera;

// The height is scaled by a scale factor s
uniform float parallaxScale = 1.0f;


vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    float height =  texture(heightMap, texCoords).r;    
    vec2 p = viewDir.xy / viewDir.z * (height * parallaxScale);
    return texCoords - p;    
} 

/*
Three components are required to compute an
offset texture coordinate for a pixel: 
  a starting texture coordinate, //texCoord
  a value of the surface height, //texCoord into heightMap
  and a tangent space vector pointing from the pixel to the eye point //viewVector

*/
mat3 createParallaxMatrix(vec3 tangent, vec3 bitangent, vec3 modelNormal)
{

  /*
  vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
  vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
  mat3 TBN = mat3(T, B, N)*/

  vec3 T = normalize(tangent);
  vec3 N = normalize(modelNormal);
  //  vec3 B = cross(N, T);
  vec3 B = normalize(bitangent);

  return mat3(T, B, N);
}


mat3 createNormalMatrix(vec3 tangent, vec3 modelNormal)
{

  /*
  vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
  vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
  mat3 TBN = mat3(T, B, N)*/

  vec3 T = normalize(tangent);
  vec3 N = normalize(modelNormal);
  T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  return mat3(T, B, N);
}

void main()
{

  ViewPosOut.xyz = fs_in.position.xyz;
  
  DiffuseOut.rgb = texture(Kdiffuse, fs_in.texCoords).rgb;
  SpecularOut.rgb = texture(Kspecular, fs_in.texCoords).rgb;

  NormalOut.rgb = normalize(fs_in.normal.rgb);

  if(normalMapping)
  {
    mat3 TBN = createNormalMatrix(fs_in.tangent.rgb, fs_in.normal.rgb);
    vec3 norm = texture(normalMap, fs_in.texCoords).rgb;
    norm = normalize(norm * 2.0f - 1.0f);
    norm = normalize(TBN * norm);
    NormalOut.rgb = norm;
  }

  if(normalMapping && parallaxMapping)
  {

    
    //Three components are required to compute an
    //offset texture coordinate for a pixel: 

    //  a starting texture coordinate, //texCoord
    //T0
    vec2 startingTexCoord = fs_in.texCoords;
    //  a value of the surface height, //texCoord into heightMap
    //h
    float height = texture(heightMap, fs_in.texCoords).r;
    //  and a tangent space vector pointing from the pixel to the eye point //viewVector
    vec3 view = normalize(camera - ViewPosOut.xyz);


    /*Since any parallax effect depends on point of view, a vector from a point on the surface to
    the eye is required.  The original texture coordinate and height values are already
    represented in tangent space, so the eye vector MUST be as well. 
    
    //Eye vector must be in tangent space as well as the texture coord and height value

    The eye vector can be
    created in global coordinates by subtracting a surface position from the eye position and
    transforming the resulting vector into tangent space.  An application programmer must
    supply a tangent, binormal, and normal at each vertex. These can be used to create a
    rotation matrix at every point on the surface which will transform vectors from the global
    coordinate system to tangent space.
    */
    
    //vec3 TangentFragPos = TBN * ViewPosOut.xyz;
    //vec3 TangentviewDir = normalize(TangentViewPos - TangentFragPos);

    //rotation matrix to scale to the normal/tangent/bitangent space
    mat3 TBN = createParallaxMatrix(fs_in.tangent, fs_in.bitangent, fs_in.normal.rgb);

    vec3 TangentViewPos = TBN * view;

    //To compute a texture coordinate offset at a point P
    // the eye vector must first be normalized to produce a normalized eye vector V (already normalized earlier)
    //the height h at the origintal texture coordinate T0
    


    // biased by a bias b 
    float b = parallaxScale * -0.5f;
  
    //new height
    float newHeight = height * parallaxScale + b;

    //new texture coordinate is used to index into a regular texture map and any other maps applied to the surface
    //Tn = T0 + (Hxb * Vx,y / Vz)
    vec2 newTexCoords = startingTexCoord + (newHeight * (view.xy / view.z));
    //vec2 newTexCoords = startingTexCoord + (newHeight * (view.xy));

    //vec2 texCoords = ParallaxMapping(fs_in.texCoords,  viewDir);

    //dont change the normal map till in the deferred shader
    DiffuseOut.rgb = texture(Kdiffuse, newTexCoords).rgb;
    SpecularOut.rgb = texture(Kspecular, newTexCoords).rgb;
    //NormalOut.rgb = fs_in.normal.rgb;

    //vec3 norm = texture(normalMap, texCoords).rgb;
    //norm = normalize(norm * 2.0f - 1.0f);
    //NormalOut.rgb = norm;

    if(newTexCoords.x > 1.0 || newTexCoords.y > 1.0 || newTexCoords.x < 0.0 || newTexCoords.y < 0.0)
      discard;
  }


  tangentOut = normalize(fs_in.tangent);
  bitangentOut = normalize(fs_in.bitangent);
  
  //AmbientOut.rgb = Kambient;

  //ViewPosOut.a = Kemissive.r;
  //NormalOut.a = Kemissive.g;
  //DiffuseOut.a = Kemissive.b;
  

  //TexCoordOut = vec3(fs_in.texCoords, 0.0); 

}
