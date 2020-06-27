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

#version 430 core
//camera position
uniform vec3 camera;
//texture coords for Kdissuse and Kspecular
// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model matrix
uniform mat4  modelMatrix;

//model matrix
uniform mat4  modelTransform;



const int maxLights = 8;
// Uniform blocks
//layout(packed) 
layout(binding = 1)uniform Global
{
  vec4 AttParam;
  vec4 FogColor; 
  vec4 Iglobal; 
  float near;
  float far;
  float Kglobal;  //global ambient color
  int activeLights;
}G;


//layout(packed) 
struct LightArray
{
  vec4 LightAmbient;   //needed for all light types
  vec4 LightDiffuse;
  vec4 LightSpecular;
  vec4 LightPosition;  //depends on the type of light
  vec4 LightDirection;

  int type;

  float outerRadius;
  float innerRadius;
  float falloffValue;
};

layout(std140, binding = 1) uniform LightBlock
{

  LightArray lights[maxLights];

}LA;

// Material values
// These values are hardcoded, but should be read from textures and passed as samplers

uniform vec3 Kemissive; //Emissive: No texture, user specified value [RGB]
uniform vec3 Kambient;  //Use a RGB vector value (specified by user through the GUI)[RGB]
//uniform float ns;     //specular shininess Use square of Ks.r as the shininess value. 

uniform sampler2D Kdiffuse;
uniform sampler2D Kspecular;

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 modelPosition;
layout(location = 1) in vec3 modelNormal;
layout(location = 2) in vec2 texCoords;
//layout(location = 3) in vec3 vertexColor;

in VS_OUT
{
  vec4 position;
  vec4 normal;
  vec2 texCoords;
} fs_in;

out vec3 color;

void main()
{
  vec3 cameraPos =camera.xyz;//;
  vec3 KdiffuseColor = texture(Kdiffuse, fs_in.texCoords).rgb;    //gets diffuse color from model
  vec3 KspecularColor = texture(Kspecular, fs_in.texCoords).rgb;  //gets specular color from model

  vec4 vertexPosition = fs_in.position;
  vec4 vertexNormal = fs_in.normal;

  //*************************************************************************************************//
  // Emissive
  vec3 Iemissive = Kemissive; //set in GUI  //WORKS

  //*************************************************************************************************//
  // Global
  vec3 finalColor = G.Kglobal * G.Iglobal.rgb + Iemissive;

  for( int i = 0; i < G.activeLights ; ++i )/*G.activeLights*/
  {
    //*************************************************************************************************//
    // Ambient
    vec3 Iambient = LA.lights[i].LightAmbient.rgb * Kambient; //Ia * Ka

    //*************************************************************************************************//
    // Diffuse    

    //Light Position * matrix
    vec4 LnotNormal =  (inverse(modelTransform) *  vec4(LA.lights[i].LightPosition.rgb, 1.0F)) - vertexPosition; // L = light pos - vert pos
    

    vec4 L = normalize(LnotNormal);                         // L = light pos - vert pos

    float NdotL = max( dot(vertexNormal, L ), 0.0f ); //L is normalized and vertNormal is normalized

    vec3 Idiffuse = LA.lights[i].LightDiffuse.rgb * KdiffuseColor * NdotL;
    
    //Light Direction * matrix
    vec4 LightDir = vec4(normalize(LA.lights[i].LightDirection.rgb),1.0f);

    const float alpha = dot(normalize(LightDir), normalize(LightDir - vertexPosition));
    /*enum LightType C++ defenition
      {
        ltNone,
        ltPoint,
        ltDirectional,
        ltSpotlight
      };*/
    float Spe = 1.0f;
    if(LA.lights[i].type == 3)
    {
      Spe = pow(
      ( ( alpha - cos(LA.lights[i].outerRadius) ) /
       ( cos(LA.lights[i].innerRadius) - cos(LA.lights[i].outerRadius) ) ) , LA.lights[i].falloffValue );
    }
    
    //*************************************************************************************************//
    // Specular (complete the implementation)
    float ns = KspecularColor.r * KspecularColor.r;  //use square of Ks.r as shininess value
    vec4 R = (2 * (NdotL) * vertexNormal) - L;  //R = 2(N.L)N - L
    vec3 V = normalize(cameraPos - vertexPosition.xyz);
    vec3 Ispecular = LA.lights[i].LightSpecular.rgb * KspecularColor * pow(max(dot(R.xyz,V), 0), ns); //Is Ks (R dot V)^ns


    // Attenutation terms (complete the implementation)
    float dL = length(LnotNormal);

    float att = min(1.0f/(G.AttParam.x + G.AttParam.y * dL + G.AttParam.z * dL * dL), 1.0f);

    // Final color
    finalColor += (att * Iambient) + (att * Spe * (Idiffuse + Ispecular));

  }

  //calculate S for fog
  float cameraDist = length(cameraPos - vertexPosition.xyz);

  float S = (G.far - cameraDist) / (G.far - G.near);

  //fog equation
  vec3 Ifinal = S * finalColor + (1.0f - S)  * G.FogColor.rgb;


  // VS outputs - position and color
  color = Ifinal;
}
//In fragment shader:
//Setup the uniforms for the application to pass in lighting properties and other settings.
//Use the values of the appropriate “in” variables in the Phong equation, if
//applicable. Note that these “in” variables MUST match the “out”
//variables from the vertex shader.
//Implement the Phong Model equations explained in class.
