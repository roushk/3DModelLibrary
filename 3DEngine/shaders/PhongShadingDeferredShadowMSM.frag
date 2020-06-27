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
uniform sampler2D gTangentMap;
uniform sampler2D gBitangentMap;
uniform sampler2D shadowMap;
uniform sampler2D blurShadowMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

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

//shadow matrix
uniform mat4  shadowMatrix;

out vec3 color;

const float alpha = 0.001f; //1 x 10^-3

uniform bool normalMapping;
uniform bool parallaxMapping;

float readShadowMap(vec3 fragPos, vec3 normal, vec3 lightDir)
{
  vec4 shadowFrag = shadowMatrix * vec4(fragPos,1);
  shadowFrag = shadowFrag/shadowFrag.w;
  shadowFrag = shadowFrag * 0.5 + 0.5;  //for some reason i have to do this twice, not sure why


  float bias = max(0.0001 * (1.0 - dot(normal, lightDir)), 0.0000001);  
  float closestDepth = texture( shadowMap, shadowFrag.xy ).r - bias;
  float currentDepth = shadowFrag.z;
 
  if(currentDepth > closestDepth)
    return 0; // depthValue;
  else 
    return 1; //shadowFrag.z;
  
}

float det3(vec3 a, vec3 b, vec3 c) // Determinant of a 3x3 passed in as three columns
 { return a.x*(b.y*c.z-b.z*c.y) + a.y*(b.z*c.x-b.x*c.z) + a.z*(b.x*c.y-b.y*c.x); }


//returns the shadow intensity
float readShadowMapMSM(vec3 fragPos, vec3 normal, vec3 lightDir)
{

  /*( A,  B, C ) * c = Z
   /  1, b1, b2 \         /   1  \ 
  |  b1, b2, b3  | * c = |   zf   |
   \ b2, b3, b4 /         \ zf^2 / 
  */
  vec4 shadowFrag = shadowMatrix * vec4(fragPos,1);
  shadowFrag = shadowFrag/shadowFrag.w;
  shadowFrag = shadowFrag * 0.5 + 0.5;  //for some reason i have to do this twice, not sure why

  //zf is the depth of the fragment
  float zf = shadowFrag.z;

  
  //float bias = max(0.0001 * (1.0 - dot(normal, lightDir)), 0.0000001);  
  //vec4 b = texture(blurShadowMap, shadowFrag.xy) - vec4(0.01f);

  //b is the blurred output fot z -> z^4
  vec4 b = texture(blurShadowMap, shadowFrag.xy );
  float max_depth = 80.0f;

  vec4 bPrime = (1 - alpha) * b + alpha * vec4(max_depth / 2.0f);

  vec3 A = vec3(1,bPrime.x, bPrime.y);
  vec3 B = vec3(bPrime.x,bPrime.y, bPrime.z);
  vec3 C = vec3(bPrime.y, bPrime.z, bPrime.w); 
  vec3 Z = vec3(1, zf, zf*zf);

  //this solves c in c1,c2,c3
  float d = det3(A,B,C);
  float c1 = det3(Z,B,C)/d;
  float c2 = det3(A,Z,C)/d;
  float c3 = det3(A,B,Z)/d;

  //vec3 c = vec3(c1,c2,c3);

  //c.z * (z^2) + c.y * z + c.x = 0;
  //ax^2 + bx + c = 0

  //return in shadow
  if((c3 == 0 ) || ((c2*c2 - (4 * c3 * c1)) == 0))
  {
    return 1.0f;
  }
  
  //z
  //c1 and c3 were flipped...
  float pos = (-c2 + sqrt(c2 * c2 - (4 * c3 * c1))) / (2 * c3);
  float neg = (-c2 - sqrt(c2 * c2 - (4 * c3 * c1))) / (2 * c3);
  
  float z2 = min(pos,neg);
  float z3 = max(pos,neg);
  float G = 0;

  //return bPrime.x;

  //if(z2 > 0.001f) return 0.0f;
  //return 1.0f;

  if(zf <= z2)  //center of shadow
  {
    G = 0;
    //return 0;
  }
  else if(zf <= z3) //in shadow somewhere
  {
    
    G = (((zf*z3) - ( ( bPrime.x * (zf + z3) ) + bPrime.y) ) /
      ( (z3 - z2) * (zf - z2) ));
      //G = 1;
    //return G;
  }
  else  //in shadow somewhere
  {
    //is returning 0
    G = 1.0f - ( ((z2*z3) - ( (bPrime.x * (z2 + z3)) + bPrime.y) ) / ((zf - z2) * (zf-z3)) );
    //return G; 
  }
  //return 0;
  //return zf;
  return 1 - G;
}

mat3 createNormalMatrix(mat4 modelMatrix, vec3 tangent, vec3 modelNormal)
{

  /*
  vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
  vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
  vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
  mat3 TBN = mat3(T, B, N)*/

  vec3 T = normalize(tangent);
  vec3 N = normalize(modelNormal);
 // T = normalize(T - dot(T, N) * N);
  vec3 B = cross(N, T);

  return mat3(T, B, N);
}

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


void main()
{
  vec3 TESTINGVALUE = vec3(0);
  //vec3 TEST = texture(normalMap, fs_in.texCoords).rgb;
  vec3 clearcolor = vec3(0.0f);
  vec2 texCoords = fs_in.texCoords;//ParallaxMapping(fs_in.texCoords,  viewDir);

  vec3 vertexPosition = texture(gPositionMap, fs_in.texCoords).xyz;
  vec3 normal = texture(gNormalMap, fs_in.texCoords).xyz;
  vec3 tangent = texture(gTangentMap, fs_in.texCoords).xyz;
  vec3 bitangent = texture(gBitangentMap, fs_in.texCoords).xyz;

  //mat3 TBN = transpose(createParallaxMatrix(tangent,bitangent, normal));


  //light, view, frag

  //vec3 norm = texture(normalMap, fs_in.texCoords).rgb;
  //norm = normalize(norm * 2.0f - 1.0f);
  //norm = normalize(TBN * norm);

  vec3 KdiffuseColor = texture(gDiffuseMap, fs_in.texCoords).rgb;

  vec3 Kspecular = texture(gSpecularMap, fs_in.texCoords).xyz;
  vec3 Kambient = vec3(0.1f,0.1f,0.1f); //texture(gAmbientMap, fs_in.texCoords).xyz;

  float ns = 32.0f; //texture(gSpecularMap, fs_in.texCoords).a;
  //Kspecular *= 0.5f;

  vec3 Kemissive;

  Kemissive.r = 0; //texture(gPositionMap, fs_in.texCoords).a;
  Kemissive.g = 0; //texture(gNormalMap, fs_in.texCoords).a;
  Kemissive.b = 0; //texture(gDiffuseMap, fs_in.texCoords).a;
  
  //update viewpos

  vec3 cameraPos = camera.xyz;//normalize(TBN *camera.xyz);
   //update vertex pos
  //vertexPosition = normalize(TBN * vertexPosition);

  vec3 vertexNormal = normal;//normal.xyz;
  
  //light position is .xyz of [3]
  //vec3 light = invShadowMatrix[3].xyz;
  
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
    //update light pos
    vec3 LnotNormal = LA.lights[i].LightPosition.xyz - vertexPosition.xyz;
//    vec3 LnotNormal = normalize(TBN * LA.lights[i].LightPosition.xyz) - vertexPosition.xyz;

    vec3 L = normalize(LnotNormal);                         // L = light pos - vert pos

    if(LA.lights[i].type != 1)  //2 or 3
      L = -normalize(LA.lights[i].LightDirection.rgb);

  
    //float NdotL = max( dot(vec4(vertexNormal,1.0f), vec4(L, 1.0f) ), 0.0f ); //L is normalized and vertNormal is normalized
    float NdotL = max( dot(vertexNormal, L), 0.0f ); //L is normalized and vertNormal is normalized

    vec3 Idiffuse = LA.lights[i].LightDiffuse.rgb * KdiffuseColor * NdotL;
    //TESTINGVALUE = LA.lights[i].LightDiffuse.rgb;
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

    //float shadow = readShadowMap(vertexPosition.xyz, vertexNormal, L);  //use view vector 
    float shadow = readShadowMapMSM(vertexPosition.xyz, vertexNormal, L);  //use view vector 


    // Final color
    finalColor += (att * Iambient) + (att * Spe * ((Idiffuse + Ispecular)));
    //finalColor += (Iambient) + (Spe * (Idiffuse + Ispecular));


    //color = LnotNormal;
    //color = L;
    //color = LA.lights[i].LightPosition.xyz;
    //color = vertexPosition.xyz;
    //color = vertexPosition.xyz;
    //color = vec3( 1.0f - shadow);
    //color = vec3(shadow);

  }

  
  //calculate S for fog
  float cameraDist = length(cameraPos - vertexPosition.xyz);

  float S = (G.far - cameraDist) / (G.far - G.near);

  //fog equation
  vec3 Ifinal = finalColor;//S * finalColor + (1.0f - S) * G.FogColor.rgb;
  

  // VS outputs - position and color
  //color = finalColor;
  
  color = Ifinal;
  //color = TESTINGVALUE;
  //color = normal;
  //color = TEST;
}

//In fragment shader:
//Setup the uniforms for the application to pass in lighting properties and other settings.
//Use the values of the appropriate “in” variables in the Phong equation, if
//applicable. Note that these “in” variables MUST match the “out”
//variables from the vertex shader.
//Implement the Phong Model equations explained in class.
