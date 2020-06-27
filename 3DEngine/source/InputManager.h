#pragma once

class InputManager
{
public:
  InputManager();
  ~InputManager();

  glm::vec3 GetArcballVector(int x, int y);
  
  void Update(float dt);

  std::string name;

  glm::vec2 mouseWorldCoords;

  int screenWidth = 1;
  int screenHeight = 1;
  bool stop = false;
  glm::vec2 mousePosition{0,0};
  glm::vec2 offset{0,0};

  //bool firstMouse = true;
  int lastY, lastX;


  bool toggleCamera = false;
  bool oldPos = false;
  float sensitivity = 0.025f;
  //float sensitivity = 0.05;


  bool pressedLeft = false;
  bool pressedRight = false;
  unsigned iteratorVal = 0;
  struct CameraMovement
  {
    bool forward = false;
    bool backward = false;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool rollLeft = false;
    bool rollRight = false;

  } cameraMovement;


};



