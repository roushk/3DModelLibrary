#ifndef GUI_H
#define GUI_H
#define PI 3.14159265359f


#include <glm/glm.hpp>


#include "Object.h"
#include "Light.h"
#include "Render.h"


/*create new light and set type with load and name and provide data*/
class GUI
{
public:

  GUI();
  ~GUI();

  void SetStyle();
  void BindImGUI(SDL_Window* window, SDL_GLContext* context);
  void RenderFrame();
  void Update(float dt = 0);

  bool buttonLeftDown = false;
  bool buttonRightDown = false;
  static bool rotateLights;
  vec3 position{0,0,0};
  static int currentCam;
  static int currentFBO;
  static int currentGJK;
  static int currentTextureMaps;

  static bool debugDrawMode;
  static bool autoCameraRotation;
  static bool rotateCamera;
  static bool showLightSpheres;
  static bool MSMShadow;
  static bool EnableLocalLights;
  static bool ParallaxMapping;
  static bool NormalMapping;
  static float ParallaxScale;
  static bool overwriteParams;
  static bool BRDF_IBL;
  static bool SSAO;
  
private:
  SceneLighting& lighting;
  ObjectReader& reader;
  Render& render;
};
#endif
