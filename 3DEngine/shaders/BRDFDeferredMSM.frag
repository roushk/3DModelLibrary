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



  //(1) choose pairs of uniformly distributed random numbers very carefully
uniform HammersleyBlock 
{
  float N;
  float hammersley[2*100]; 
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
uniform sampler2D shadowMap;
uniform sampler2D blurShadowMap;
uniform sampler2D skydomeTexture;
uniform sampler2D skydomeIRR;
uniform sampler2D SSAOBlurMap;


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

const float PI = 3.1415926535897932384626433832795;

const float MSMalpha = 0.001f; //1 x 10^-3

uniform float materialAlpha;

uniform float max_depth;
uniform float scalarLevel;
uniform float exposure;
uniform float contrast;

const int totalSamples = 20;

uniform bool toggleSSAO;

out vec3 color;

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

  vec4 bPrime = (1 - MSMalpha) * b + MSMalpha * vec4(max_depth / 2.0f);

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

//input = input color, exposure = range 0.1 to 10000, contrast is additional
vec3 sRGBtoLinear(vec3 input, float exposure, float contrast)
{
  return pow(( (exposure * input) / (exposure * input + vec3(1,1,1)) ), vec3(contrast/2.2f));
}

vec3 LineartosRGB(vec3 input, float exposure, float contrast)
{
  return pow(( (exposure * input) / (exposure * input + vec3(1,1,1)) ), vec3(1.0f/(contrast/2.2f)));
}

void main()
{
  //set variables

  vec3 clearcolor = vec3(0.0f);
  vec3 vertexPosition = texture(gPositionMap, fs_in.texCoords).xyz;
  vec3 normal = texture(gNormalMap, fs_in.texCoords).xyz;
  vec3 KdiffuseColor = texture(gDiffuseMap, fs_in.texCoords).xyz;
  vec3 Kspecular = texture(gSpecularMap, fs_in.texCoords).xyz;
  vec3 Kambient = texture(gAmbientMap, fs_in.texCoords).xyz; //vec3(0.1f,0.1f,0.1f); 
  float SSAO = texture(SSAOBlurMap, fs_in.texCoords).r;

  float ns = 32.0f; //texture(gSpecularMap, fs_in.texCoords).a;
  //Kspecular *= 0.5f;

  vec3 Kemissive;

  Kemissive.r = 0; //texture(gPositionMap, fs_in.texCoords).a;
  Kemissive.g = 0; //texture(gNormalMap, fs_in.texCoords).a;
  Kemissive.b = 0; //texture(gDiffuseMap, fs_in.texCoords).a;
  

  
  vec3 cameraPos = camera.xyz;
  vec3 vertexNormal = normal.xyz;
  vec3 V = normalize(cameraPos - vertexPosition.xyz);
  //*************************************************************************************************//
  // Emissive
  vec3 Iemissive = Kemissive; //set in GUI  //WORKS

  //*************************************************************************************************//
  // Global
  vec3 finalColor = vec3(0,0,0);  //G.Kglobal * G.Iglobal.rgb + Iemissive;

  vec3 TEST_VALUE = vec3(0);
  //*************************************************************************************************//
  // IBL Calculations
  //had to flip y and z because I use a Y up  
  vec2 uv = vec2((0.5f - ( atan(vertexNormal.z, vertexNormal.x) / ( 2.0f * PI))), acos(vertexNormal.y) / PI);
  //uv.x = min(1.0f,max(0.0f, uv.x));
  //uv.y = min(1.0f,max(0.0f, uv.y));
  
  vec3 skydomeTex = texture(skydomeTexture, uv).rgb;
  vec3 skydomeTexIRR = sRGBtoLinear(texture(skydomeIRR, uv).rgb, exposure, contrast);

  //color = vec3(1,0,0);
  vec2 skewDistrib[totalSamples];
  vec3 skewDirVec[totalSamples];

  //(2) skew the points to match the distribution
  for(uint i = 0; i < totalSamples * 2; ++i)
  {
    skewDistrib[i / 2].x = hammersley[i];
    skewDistrib[i / 2].y = acos( pow(hammersley[i + 1], (1.0f / (materialAlpha + 1.0f)) ))/PI;
  }

  for(uint i = 0; i < totalSamples; ++i)
  {
    float u = skewDistrib[i].x;
    float v = skewDistrib[i].y;
    //(3) form the direction vector L for that pair using EQN 4
    skewDirVec[i].x = cos(2 * PI * (0.5 - u)) * sin(PI * v);
    skewDirVec[i].y = sin(2 * PI * (0.5 - u)) * sin(PI * v);
    skewDirVec[i].z = cos(PI * v);
    skewDirVec[i] = normalize(skewDirVec[i]);

  }
  float totalWeight = 0;

  //(4) Form a rotation that takes the Z axis to the reflection direction R = 2 * (N*V)N - V
  vec3 R = (2 * dot(vertexNormal, V) * vertexNormal) - V;
  vec3 A = normalize(cross(vec3(0,0,1),R)); //tangent 
  //A = normalize(vec3(-R.y, R.x, 0));
  vec3 B = normalize(cross(R,A));           //bitangent
  vec3 IBL = vec3(0);


  //omega_k = normal + random variation
  //Li = light brightnesss
  //cos(theta) = N * L
  for(uint i = 0; i < totalSamples; ++i)
  {
    //wk = omegak
    //omega_k = normalize(L.x * A + L.y * B + L.z * R);
    vec3 omegaK = normalize(skewDirVec[i].x * A + skewDirVec[i].y * B + skewDirVec[i].z * R);

    //get UV coordinates using eqn (4.a) for the specular map
    vec2 uv_IBL = vec2((0.5f - ( atan(omegaK.z, omegaK.x) / ( 2.0f * PI))), acos(omegaK.y) / PI);

    //calculate H as the halfway between light vector (omega_k) and the view vector V
    vec3 H_IBL = normalize(omegaK + V);

    //calculate D_IBl as
    float DH_IBL = ((materialAlpha + 2.0f) / (2.0f*PI)) * (pow(max(dot(vertexNormal,H_IBL), 0), materialAlpha));
    
    //for each omega_k calculated by each light Li(omega_k)
    //float level = 1.0f + ( (0.5f * log2(2048.0f*1024.0f / totalSamples)) - (log2( 0.5f * 2.0f DH_IBL)) );  //2048.0f*1024.0f

    float level = 1.0f + ( (0.5f * log2(2048.0f*1024.0f / totalSamples)) - (log2(  DH_IBL / 4.0f)) );  //2048.0f*1024.0f
    
    //vec3 specular = texture(skydomeTexture, uv_IBL).rgb;
    vec3 specular = textureLod(skydomeTexture, uv_IBL, level).rgb;
    //if(materialAlpha > 6000)
    //  specular = texture(skydomeTexture, uv).rgb;

    
    vec3 F_IBL = Kspecular + ((1 - Kspecular) * pow((1 - dot(omegaK,H_IBL)),5) );
    
    //G(omegaK, V, H)
    float G_IBL = 1.0f;//1.0f / ( dot(omegaK,H_IBL) * dot(omegaK,H_IBL) ); 
    
    float NdotL_IBL = max(dot(omegaK, vertexNormal),0);//skewDirVec[i]);

    vec3 BRDF_IBL = ((F_IBL * G_IBL) / (4.0f)) * specular * NdotL_IBL;

    //totalWeight += NdotL_IBL;
    IBL += BRDF_IBL;
    //TEST_VALUE = vec3(level / 100);
  }
  //TEST_VALUE /= totalWeight;
  IBL /= totalSamples;


  vec3 IBLDiffuse = (KdiffuseColor / PI ) * skydomeTexIRR;
  IBL += IBLDiffuse;
  

  //apply SSAO to ambient terms
  if(toggleSSAO)
  {
    IBL *= SSAO;
  }
  // = (KdiffuseColor / PI ) * skydomeTexIRR + totalhammersleyColor;

  //Frensel Term F
  //Ks + ( (w - Ks) * (1- dot(L,H))^5 )
  //F(L,H) = Ks + ( (1 - Ks) * (1- dot(L,H))^5 )
  //replace specular with sampling method

  //vec3 F = Kspecular + ((1 - Kspecular) * pow((1 - dot(L,H)),5) );


  //G(L,V,H)) / ( dot(L,N) * dot(V,N) ) = 1 / ( dot(L,H) ^ 2) = 1

  //BRDF = f(L,V,N) = Kd/Pi + (D(H) * F(L,H) * G(L,V,H)) / (4 * ( dot(L,N) * dot(V,N) ))
  //BRDF = f(L,V,N) = Kd/Pi + (D(H) * F(L,H) * G(L,V,H)) / (4 * ( dot(L,vertexNormal) * dot(V,vertexNormal) ))
  //skydomeTex = irradience map is working (Very hard to see)
  //vec3 specularPortion = ((D * F * G) / (4.0f));

      

  //Your light sources may be one or more global lights PLUS the IBL environment light.
  //final color = sum of color from all lights + IBL
  finalColor += IBL;
  //finalColor = TEST_VALUE;
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

    //vec3 Idiffuse = LA.lights[i].LightDiffuse.rgb * KdiffuseColor * NdotL;
    
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
    
    //vec3 Ispecular = LA.lights[i].LightSpecular.rgb * Kspecular * pow(max(dot(V,ReflectVec), 0), ns); //Is Ks (R dot V)^ns
 

    // Attenutation terms (complete the implementation)
    float dL = length(LnotNormal);

    float att = min(1.0f/(G.AttParam.x + G.AttParam.y * dL + G.AttParam.z * dL * dL), 1.0f);

    //float shadow = readShadowMap(vertexPosition.xyz, vertexNormal, L);  //use view vector 
    float shadow = readShadowMapMSM(vertexPosition.xyz, vertexNormal, L);  //use view vector 
    
    //H = (L + V) / ||(L + V)||
    //H = normalize(L+V);
    vec3 H = normalize(L + V);

    //Roughness Parameter D
    //materialAlpha 0 = rough -> inf = mirror
    //D(H) = ( (alpha  + 2) / (2*PI) ) * (dot(N,H) ^ alpha)

    //float D = ((materialAlpha + 2.0f) / (2.0f*PI)) * (pow(dot(vertexNormal,H), materialAlpha));
    float D = ((materialAlpha + 2.0f) / (2.0f*PI)) * (pow(max(dot(vertexNormal,H),0), materialAlpha));

    //Frensel Term F
    //Ks + ( (w - Ks) * (1- dot(L,H))^5 )
    //F(L,H) = Ks + ( (1 - Ks) * (1- dot(L,H))^5 )
    //replace specular with sampling method

    //vec3 F = Kspecular + ((1 - Kspecular) * pow((1 - dot(L,H)),5) );

    vec3 F = Kspecular + ((1 - Kspecular) * pow((1 - dot(L,H)),5) );

    //G(L,V,H)) / ( dot(L,N) * dot(V,N) ) = 1 / ( dot(L,H) ^ 2) = 1
    float G = 1.0f / ( dot(L,H) * dot(L,H) ); 

    //BRDF = f(L,V,N) = Kd/Pi + (D(H) * F(L,H) * G(L,V,H)) / (4 * ( dot(L,N) * dot(V,N) ))
    //BRDF = f(L,V,N) = Kd/Pi + (D(H) * F(L,H) * G(L,V,H)) / (4 * ( dot(L,vertexNormal) * dot(V,vertexNormal) ))
    //skydomeTex = irradience map is working (Very hard to see)
    //vec3 specularPortion = ((D * F * G) / (4.0f));

        
    vec3 BRDF = (KdiffuseColor / PI ) + ((D * F * G) / (4.0f));

    //BRDF * light Brightness * Shadow
    finalColor += (att * Spe * (BRDF * LA.lights[i].LightDiffuse.rgb));
    //finalColor += (att * Spe * (BRDF * LA.lights[i].LightDiffuse.rgb * shadow));

    //finalColor += (Iambient) + (Spe * (Idiffuse + Ispecular));
    //color = LnotNormal;
    //color = L;
    //color = LA.lights[i].LightPosition.xyz;
    //color = vertexPosition.xyz;
    //color = vertexPosition.xyz;
    //color = vec3( 1.0f - shadow);
    //color = vec3(shadow);
    //color = totalhammersleyColor;
  }

  //calculate S for fog
  float cameraDist = length(cameraPos - vertexPosition.xyz);

  float S = (G.far - cameraDist) / (G.far - G.near);

  //fog equation
  //vec3 Ifinal = finalColor;//S * finalColor + (1.0f - S) * G.FogColor.rgb;
  

  // VS outputs - position and color
  //color = finalColor;
  color = sRGBtoLinear(finalColor, exposure, contrast);
  
  //color = IBL;
  //color = IBL;
  //color = TEST_VALUE;
  //color = skydomeTexIRR;
  //color = skydomeTex;
}
