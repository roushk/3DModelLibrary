/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.

File Name: PhongShadingAndFrensel.frag
Purpose: Implementation of phong shader
Language: C++ MSVC
Platform: GLSL, OpenGL 4.3
Project: coleman.jonas_CS300_4
Author: Coleman Jonas coleman.jonas 280003516
Creation date: 7/24/18
End Header --------------------------------------------------------*/

#version 430 core

uniform vec3 camera;
//texture coords for Kdissuse and Kspecular
// This matrix holds the model-to-view transform
uniform mat4  viewMatrix;

// This matrix holds the projection matrix
uniform mat4  projectionMatrix;

//model matrix
uniform mat4  modelTransform;

//model matrix
uniform mat4  modelMatrix;



layout(binding = 2)uniform sampler2D positiveX;  //face 0
layout(binding = 3)uniform sampler2D negativeX;  //face 1

layout(binding = 4)uniform sampler2D positiveY;  //face 2
layout(binding = 5)uniform sampler2D negativeY;  //face 3

layout(binding = 6)uniform sampler2D positiveZ;  //face 4
layout(binding = 7)uniform sampler2D negativeZ;  //face 5


in VS_OUT
{
  vec4 position;
  vec4 normal;
  vec2 texCoords;
} fs_in;


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

uniform float highlightTightness;  //K
uniform float transCoeff;

out vec3 color;

vec3 Reflect()
{

  vec3 normal = fs_in.normal.xyz;

  vec3 position = fs_in.position.xyz;
  vec3 viewVector = normalize(position - camera); // L = light pos - vert pos

  //R = 2 * (N*V)N - V
  vec3 reflectVector = (2.0f *  dot(normal, viewVector) * normal) - viewVector; 
 
  reflectVector = -reflectVector; //the entire reflection is inverted if i dont
 
  vec3 fragColor = vec3(-1,-1,-1);

  vec2 uv = vec2(0,0);

  float absX = abs(reflectVector.x);
  float absY = abs(reflectVector.y);
  float absZ = abs(reflectVector.z);

  bool XPositive = reflectVector.x > 0 ? true : false;
  bool YPositive = reflectVector.y > 0 ? true : false;
  bool ZPositive = reflectVector.z > 0 ? true : false;

  float largestLine = 0;
  int face = -1;
  


  if (XPositive && absX >= absY && absX >= absZ) //X+
  {
    face = 0;
    largestLine = absX;
    uv.x = reflectVector.z; 
    uv.y = reflectVector.y;
  }
  if (XPositive == false && absX >= absY && absX >= absZ) //X-
  {
    face = 1;
    largestLine = absX;
    uv.x = -reflectVector.z; 
    uv.y = reflectVector.y;
  }

  if (YPositive && absY >= absX && absY >= absZ) 
  {
    face = 2;
    largestLine = absY;
    uv.x = -reflectVector.x;
    uv.y = -reflectVector.z;
  }
  if (YPositive == false && absY >= absX && absY >= absZ) 
  {
    face = 3;
    largestLine = absY;
    uv.x = -reflectVector.x;
    uv.y = reflectVector.z;
  }

  if (ZPositive && absZ >= absX && absZ >= absY) //Z+
  {
    face = 4;
    largestLine = absZ;
    uv.x = -reflectVector.x; 
    uv.y = reflectVector.y;
  }
  if (ZPositive == false && absZ >= absX && absZ >= absY) //Z-
  {
    face = 5;
    largestLine = absZ;
    uv.x = reflectVector.x; 
    uv.y = reflectVector.y;
  }

  uv.x = 0.5f * (uv.x / largestLine + 1.0f);
  uv.y = 0.5f * (uv.y / largestLine + 1.0f);
  
  


  if(face == 0)
    fragColor = texture(positiveX, uv).rgb; 
  if(face == 1)
    fragColor = texture(negativeX, uv).rgb; 
  if(face == 2)
    fragColor = texture(positiveY, uv).rgb; 
  if(face == 3)
    fragColor = texture(negativeY, uv).rgb; 
  if(face == 4)
    fragColor = texture(positiveZ, uv).rgb; 
  if(face == 5)
    fragColor = texture(negativeZ, uv).rgb; 

  return fragColor;
}

vec4 Refract(float transCoeffpassed, inout vec3 viewVector, inout vec3 normal)
{

  vec3 cameraPos = camera.xyz;
  vec3 position = fs_in.position.xyz;

  vec3 finalColor = vec3(0,0,0);


  vec3 newNormal = normal;
  //vec3 cameraPos = (modelPosition * vec4(camera,1)).xyz;

  //R = 2 * (N*V)N - V
  
  float NdotL = dot(newNormal,viewVector);
  //T = [ K*(N*L) - sqrt(1-(k * k)(1-(N*L * N*L)) )] * N - K.L
  vec3 transVector;
  float newTransCoeff = transCoeffpassed;
  if(NdotL < 0) //outside surface
  {
    NdotL = -NdotL;
  }
  else  //inside the surface
  {
    newNormal = -newNormal;

    newTransCoeff = 1.0f/newTransCoeff; //flip coeff cause relationship between outer and inner material is flipped
  }

  float c2 = (sqrt(1.0f - (newTransCoeff * newTransCoeff * (1.0f - (NdotL * NdotL)))));

  //transVector = (newTransCoeff * NdotL - c2) * newNormal - (newTransCoeff * viewVector);
  
  //c1 = NdotL
  //c2 = sqrt(1-(coeff^2)(1-NdotL^2))
  
  transVector = newTransCoeff * viewVector + (newTransCoeff * NdotL - c2) * newNormal;
  //transVector = refract(viewVector, normal, transCoeff);
  transVector = normalize(transVector);
  //LA.lights[i].LightDiffuse.xyz
  //vec3 Irefract = fragColor.xyz * transCoeff * pow(max(dot(viewVector, transVector), 0), highlightTightness);

  //transVector = -transVector;
  vec3 fragColor = vec3(-1,-1,-1);

  vec2 uv = vec2(0,0);

  float absX = abs(transVector.x);
  float absY = abs(transVector.y);
  float absZ = abs(transVector.z);

  bool XPositive = transVector.x > 0 ? true : false;
  bool YPositive = transVector.y > 0 ? true : false;
  bool ZPositive = transVector.z > 0 ? true : false;

  float largestLine = 0;
  int face = -1;
  


  if (XPositive && absX >= absY && absX >= absZ) //X+
  {
    face = 0;
    largestLine = absX;
    uv.x = transVector.z; 
    uv.y = transVector.y;
  }
  if (XPositive == false && absX >= absY && absX >= absZ) //X-
  {
    face = 1;
    largestLine = absX;
    uv.x = -transVector.z; 
    uv.y = transVector.y;
  }

  if (YPositive && absY >= absX && absY >= absZ) 
  {
    face = 2;
    largestLine = absY;
    uv.x = -transVector.x;
    uv.y = -transVector.z;
  }
  if (YPositive == false && absY >= absX && absY >= absZ) 
  {
    face = 3;
    largestLine = absY;
    uv.x = -transVector.x;
    uv.y = transVector.z;
  }

  if (ZPositive && absZ >= absX && absZ >= absY) //Z+
  {
    face = 4;
    largestLine = absZ;
    uv.x = -transVector.x; 
    uv.y = transVector.y;
  }
  if (ZPositive == false && absZ >= absX && absZ >= absY) //Z-
  {
    face = 5;
    largestLine = absZ;
    uv.x = transVector.x; 
    uv.y = transVector.y;
  }

  uv.x = 0.5f * (uv.x / largestLine + 1.0f);
  uv.y = 0.5f * (uv.y / largestLine + 1.0f);
  
  


  if(face == 0)
    fragColor = texture(positiveX, uv).rgb; 
  if(face == 1)
    fragColor = texture(negativeX, uv).rgb; 
  if(face == 2)
    fragColor = texture(positiveY, uv).rgb; 
  if(face == 3)
    fragColor = texture(negativeY, uv).rgb; 
  if(face == 4)
    fragColor = texture(positiveZ, uv).rgb; 
  if(face == 5)
    fragColor = texture(negativeZ, uv).rgb; 

  return vec4(fragColor * newTransCoeff * pow(max(dot(viewVector, transVector), 0), highlightTightness), newTransCoeff);
}

vec4 ChromaticAbb(inout vec3 viewVector, inout vec3 normal)
{
  //color = transVector.xyz;
  float etaR = 0.65f/0.66f;
  float etaG = 0.67f/0.66f;
  float etaB = 0.69f/0.66f;

  vec4 newColor;
  newColor.r = Refract(etaR * transCoeff, viewVector, normal).r;
  newColor.g = Refract(etaG * transCoeff, viewVector, normal).g;
  newColor.b = Refract(etaB * transCoeff, viewVector, normal).b;
  newColor.a = Refract(transCoeff, viewVector, normal).a;
  return newColor;
}

void main()
{

  vec3 cameraPos = camera.xyz;//;
  vec3 KdiffuseColor = texture(Kdiffuse, fs_in.texCoords).rgb;    //gets diffuse color from model
  vec3 KspecularColor = texture(Kspecular, fs_in.texCoords).rgb;  //gets specular color from model

  vec4 vertexPosition = fs_in.position;
  vec4 vertexNormal = fs_in.normal;

  //*************************************************************************************************//
  // Emissive
  vec3 Iemissive = Kemissive; //set in GUI  //WORKS

  //*************************************************************************************************//
  // Global

  vec3 viewVector = normalize(fs_in.position.xyz - camera.xyz); // L = light pos - vert pos
  vec3 normal = fs_in.normal.xyz;
/*************************************************************************************/
//Reflection
  vec3 reflectColor = Reflect();

/*************************************************************************************/
//Refraction

  vec4 refractColor = ChromaticAbb(viewVector, normal);

/*************************************************************************************/
//Frensel
  float Eta = refractColor.a;
  const float FresnelPower = 5.0;
  const float F = ((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta));
  
  float ratio = F + (1.0 - F) * pow((1.0 - dot(-viewVector, normal)), FresnelPower);

  vec3 frenselColor = mix(refractColor.rgb, reflectColor, ratio);
/*************************************************************************************/
//Lighting

//*************************************************************************************************//
// Global
  vec3 finalColor = G.Kglobal * G.Iglobal.rgb + Iemissive * frenselColor;

  for( int i = 0; i < G.activeLights ; ++i )/*G.activeLights*/
  {
    //*************************************************************************************************//
    // Ambient
    vec3 Iambient = LA.lights[i].LightAmbient.rgb * Kambient; //Ia * Ka

    //*************************************************************************************************//
    // Diffuse    

    //Light Position * matrix
    vec4 LnotNormal = (inverse(modelTransform) * vec4(LA.lights[i].LightPosition.rgb, 1.0F)) - vertexPosition; // L = light pos - vert pos
    

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
  //color = Kambient;
  //color = ReflectionColor();
}
 
 
 