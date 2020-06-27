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
//camera position
uniform vec3 camera;

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

uniform sampler2D gPositionMap; //position map.a = emi
uniform sampler2D gNormalMap;
uniform sampler2D gDiffuseMap;
uniform sampler2D gSpecularMap;
uniform sampler2D gAmbientMap;

in VS_OUT
{
  vec2 texCoords;
} fs_in;

// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model matrix
uniform mat4  modelMatrix;
//model matrix
uniform mat4  modelTransform;


out vec3 color;

void main()
{
  
  vec3 clearcolor = vec3(0.0f);
  vec3 vertexPosition = texture(gPositionMap, fs_in.texCoords).xyz;
  vec3 normal = texture(gNormalMap, fs_in.texCoords).xyz;
  vec3 KdiffuseColor = texture(gDiffuseMap, fs_in.texCoords).xyz;

  vec3 Kspecular = texture(gSpecularMap, fs_in.texCoords).xyz;
  vec3 Kambient = vec3(0,0,0); //texture(gAmbientMap, fs_in.texCoords).xyz;

  float ns = 32.0f; //texture(gSpecularMap, fs_in.texCoords).a;
  //Kspecular *= 0.5f;

  vec3 Kemissive;

  Kemissive.r = 0; // texture(gPositionMap, fs_in.texCoords).a;
  Kemissive.g = 0; //texture(gNormalMap, fs_in.texCoords).a;
  Kemissive.b = 0; //texture(gDiffuseMap, fs_in.texCoords).a;
  

  vec3 cameraPos = camera.xyz;
  vec3 vertexNormal = normal.xyz;

/*
  if(viewPos == clearcolor && normal == clearcolor )
  {
    color = vec3(0.2f,0.2f,0.2f);//discard;
    return;
  }
  */
  //eyeVec = eye - worldPos;
  //V = cameraPos - vertexPosition.xyz; = cam - V = viewPos
  vec3 V = normalize(cameraPos - vertexPosition.xyz);
  //*************************************************************************************************//
  // Emissive
  vec3 Iemissive = Kemissive; //set in GUI  //WORKS

  //*************************************************************************************************//
  // Global
  vec3 finalColor = vec3(0,0,0);  //G.Kglobal * G.Iglobal.rgb + Iemissive;

  for( int i = 0; i < G.activeLights ; ++i )/*G.activeLights*/
  {
    //*************************************************************************************************//
    // Ambient
    vec3 Iambient = LA.lights[i].LightAmbient.rgb * Kambient; //Ia * Ka

    //*************************************************************************************************//
    // Diffuse    

    //Light Position * matrix
    // L = light pos - vert pos
    //lightVec = lightPos - worldPos;
    //vec4 lightPos = LA.lights[i].LightPosition
    vec3 LnotNormal = LA.lights[i].LightPosition.xyz - vertexPosition.xyz;

    vec3 L = normalize(LnotNormal);                         // L = light pos - vert pos

    if(LA.lights[i].type != 1)  //2 or 3
      L = -normalize(LA.lights[i].LightDirection.rgb);

  
    //float NdotL = max( dot(vec4(vertexNormal,1.0f), vec4(L, 1.0f) ), 0.0f ); //L is normalized and vertNormal is normalized
    float NdotL = max( dot(vertexNormal, L), 0.0f ); //L is normalized and vertNormal is normalized

    vec3 Idiffuse = LA.lights[i].LightDiffuse.rgb * KdiffuseColor * NdotL;
    
    //Light Direction * matrix
    //vec4 LightDir = vec4(normalize(LA.lights[i].LightDirection.rgb),1.0f);

    //vec3 ReflectVec = normalize((((2.0f * NdotL) * vertexNormal) - L));  //ReflectVec = 2(N.L)N - L
    vec3 ReflectVec = reflect(-L, vertexNormal);  //ReflectVec = 2(N.L)N - L

  
    float Spe = 1.0f;
    if(LA.lights[i].type == 3)
    {
      const float alpha = dot(L, ReflectVec);
      Spe = pow(
      ( ( alpha - cos(LA.lights[i].outerRadius) ) /
       ( cos(LA.lights[i].innerRadius) - cos(LA.lights[i].outerRadius) ) ) , LA.lights[i].falloffValue );
    }
    
    //*************************************************************************************************//
    // Specular (complete the implementation)
    
    vec3 Ispecular = LA.lights[i].LightSpecular.rgb * Kspecular * pow(max(dot(V,ReflectVec), 0), ns); //Is Ks (R dot V)^ns
 

    // Attenutation terms (complete the implementation)
    float dL = length(LnotNormal);

    float att = min(1.0f/(G.AttParam.x + G.AttParam.y * dL + G.AttParam.z * dL * dL), 1.0f);

    // Final color
    finalColor += (att * Iambient) + (att * Spe * (Idiffuse + Ispecular));
    //finalColor += (Iambient) + (Spe * (Idiffuse + Ispecular));


    //color = LnotNormal;
    //color = L;
    //color = LA.lights[i].LightPosition.xyz;
    //color = vertexPosition.xyz;
    //color = vertexPosition.xyz;
  
  }

  
  //calculate S for fog
  float cameraDist = length(cameraPos - vertexPosition.xyz);

  float S = (G.far - cameraDist) / (G.far - G.near);

  //fog equation
  vec3 Ifinal = finalColor;//S * finalColor + (1.0f - S) * G.FogColor.rgb;
  

  // VS outputs - position and color
  //color = finalColor;
  
  color = Ifinal;
  
  //color = normalize(LA.lights[0].LightPosition.xyz - vertexPosition.xyz);//LA.lights[0].LightPosition.xyz;
}

//In fragment shader:
//Setup the uniforms for the application to pass in lighting properties and other settings.
//Use the values of the appropriate “in” variables in the Phong equation, if
//applicable. Note that these “in” variables MUST match the “out”
//variables from the vertex shader.
//Implement the Phong Model equations explained in class.
