#include <pch.h>





InputManager::InputManager()
{
  name = "Input Manager";
  lastX = mousePosition.x;
  lastY = mousePosition.y;
}

InputManager::~InputManager()
{

}

//https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
glm::vec3 InputManager::GetArcballVector(int x, int y)
{
  auto& render = pattern::get<Render>();

  glm::vec3 P = glm::vec3(1.0*x / render.windowX * 2 - 1.0,
    1.0*y / render.windowY * 2 - 1.0,
    0);

  P.y = -P.y;
  float OP_squared = P.x * P.x + P.y * P.y;
  if (OP_squared <= 1 * 1)
    P.z = sqrt(1 * 1 - OP_squared);  // Pythagoras
  else
    P = glm::normalize(P);  // nearest point
  return P;
}

void InputManager::Update(float dt)
{
  //stops camera mouse from jumping after unclicking right clock
  //this is caused by the input not being calculated with each type all at once and
  //instead buffers the clicks in different order and causes the toggle camera
  //update thing to be called twice. The solution to this is to make a real
  //input manager but this works for now...
  bool updatedThisLoop = false;




  auto& render = pattern::get<Render>();
  auto& input = pattern::get<InputManager>();
  auto& gui = pattern::get<GUI>();
  auto& reader = pattern::get<ObjectReader>();
  auto& camera = render.currentCamera;

  //update window size
  SDL_GetWindowSize(render.gWindow, &render.windowX, &render.windowY);

  //update window position
  int windowXPos = 0;
  int windowYPos = 0;
  SDL_GetWindowPosition(render.gWindow, &windowXPos, &windowYPos);
  render.windowPosition = glm::vec2(windowXPos, windowYPos);
  
  auto& object = pattern::get<Model>();

  if (pressedLeft && gui.buttonLeftDown == false)
  {
    
    ++iteratorVal;
    iteratorVal = iteratorVal % reader.models.size();
    object = reader.GetObject(iteratorVal);

    render.SetTitle(object.name);
    pressedLeft = false;
    
  }
  if (gui.buttonLeftDown == true)
  {
    pressedLeft = true;
  }
  if (pressedRight && gui.buttonRightDown == false)
  {
    
    --iteratorVal;
    iteratorVal += reader.models.size(); //incase 0
    iteratorVal = iteratorVal % reader.models.size();
    object = reader.GetObject(iteratorVal);

    render.SetTitle(object.name);
    pressedRight = false;
    
  }
  if (gui.buttonRightDown == true)
  {
    pressedRight = true;
  }

  //toggle between normal, color, none
  /*
  if (pressedB && glfwGetKey(render.window, GLFW_KEY_B) == GLFW_RELEASE)
  {
    render.setting++;
    render.setting = render.setting % 3;

    render.SetTitle(object.name);
    pressedB = false;
  }
  */
 
  const float speed = 3.0f * dt;

  SDL_Event event;
  //while event queue is not empty pop off and deal with
  while (SDL_PollEvent(&event))
  {
    ImGui_ImplSDL2_ProcessEvent(&event);
    switch (event.type)
    {
    default:
      //default event
      break;
      //if you press the close button the game shuts down the engine
    case SDL_QUIT:
    {
      stop = true;
    }
    break;

    case SDL_WINDOWEVENT:
    {
      if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        //|| event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) //breaks everything called ALOT

      {
        render.resize(event.window.data1, event.window.data2);
        
      }
      else if (event.window.event == SDL_WINDOWEVENT_MAXIMIZED)
      {

      }

    }

    break;
    /*SDL_KeyboardEvent*/
    case SDL_KEYDOWN:
    {

      float cameraAccelSpeed = 10.0f;
      
      
      if (event.key.keysym.scancode == SDL_SCANCODE_W 
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.forward = true;
      }

      
      
      if (event.key.keysym.scancode == SDL_SCANCODE_A
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.left = true;

      }
      

      if (event.key.keysym.scancode == SDL_SCANCODE_S
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.backward = true;
      }

     

      if (event.key.keysym.scancode == SDL_SCANCODE_D
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.right = true;

      }




      if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.up = true;

      }



      if (event.key.keysym.scancode == SDL_SCANCODE_LCTRL
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.down = true;

      }



     

      if (event.key.keysym.scancode == SDL_SCANCODE_Q
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.rollLeft = true;

      }




      if (event.key.keysym.scancode == SDL_SCANCODE_E
        && event.key.state == SDL_PRESSED)
      {
        cameraMovement.rollRight = true;
      }



      
      //engine.GetSystem<Render>()->cameraPos =
      //  glm::translate(engine.GetSystem<Render>()->cameraPos, glm::vec3(cameraPos, 0));
    }
    break;
    case SDL_KEYUP:

      if (event.key.keysym.scancode == SDL_SCANCODE_F
        && event.key.state == SDL_RELEASED)
      {
        camera.ResetRoll();
        render.cameraChanged = true;

      }

      if (event.key.keysym.scancode == SDL_SCANCODE_W
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.forward = false;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_A
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.left = false;
      }

      if (event.key.keysym.scancode == SDL_SCANCODE_S
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.backward = false;
      }
      if (event.key.keysym.scancode == SDL_SCANCODE_D
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.right = false;

      }
      if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.up = false;

      }
      if (event.key.keysym.scancode == SDL_SCANCODE_LCTRL
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.down = false;

      }
      if (event.key.keysym.scancode == SDL_SCANCODE_Q
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.rollLeft = false;

      }
      if (event.key.keysym.scancode == SDL_SCANCODE_E
        && event.key.state == SDL_RELEASED)
      {
        cameraMovement.rollRight = false;
      }

      break;

      /*SDL_MouseButtonEvent*/
    case SDL_MOUSEBUTTONDOWN:
    {
      if (event.button.button == SDL_BUTTON_RIGHT && event.button.state == SDL_PRESSED
         && updatedThisLoop == false)
      //&& event.button.type == SDL_MOUSEBUTTONDOWN)
      {
        updatedThisLoop = true;
        if (toggleCamera == false)
        {
          toggleCamera = true;
          SDL_SetRelativeMouseMode(SDL_bool(true));
        }
        else if (toggleCamera == true)
        {
          toggleCamera = false;
          SDL_SetRelativeMouseMode(SDL_bool(false));

          glm::vec2 windowRes = glm::vec2(render.height*render.aspect, render.height);

          SDL_WarpMouseGlobal(render.windowPosition.x + windowRes.x / 2,
            render.windowPosition.y + windowRes.y / 2);
        }

      }

    }
    break;
    case SDL_MOUSEBUTTONUP:
    {
      //Drag select
      if (event.button.button == SDL_BUTTON_LEFT
        && event.button.state == SDL_RELEASED)
      {

      }
    }
    break;
    /*SDL_MouseWheelEvent*/
    case SDL_MOUSEWHEEL:
    {
      float scale = 1.0f;
      if (event.wheel.y > 0) // scroll up
      {
        camera.zoom(0.9f);
        render.cameraChanged = true;
        // Pull up code here!
      }
      else if (event.wheel.y < 0) // scroll down
      {
        camera.zoom(1.1f);
        render.cameraChanged = true;
        // Pull down code here!
      }
      //= glm::scale(engine.GetSystem<Render>()->cameraPos, glm::vec3(scale, scale, 1.0f));
    }
    break;
    /*SDL_MouseMotionEvent*/
    case SDL_MOUSEMOTION:
    {
      //updates mouse world and mouse screen coords

      //mouseScreenCoords = glm::vec2((-screenSize.x / 2.0f + event.motion.x) * aspect, screenSize.y / 2.0f + -event.motion.y);
      offset += glm::vec2(event.motion.xrel, event.motion.yrel);
      mousePosition = glm::vec2{ event.motion.x, event.motion.y } + offset;
    }
    break;
    }
  }

  

  float xoffset = mousePosition.x - lastX;
  float yoffset = lastY - mousePosition.y;
  lastX = mousePosition.x;
  lastY = mousePosition.y;

  if (toggleCamera)
  {
    //render.cameraChanged = true;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera.pitch(yoffset);
    camera.yaw(xoffset);

    glm::vec2 windowRes = glm::vec2(render.height*render.aspect, render.height);

    SDL_WarpMouseGlobal(render.windowPosition.x + windowRes.x / 2,
      render.windowPosition.y + windowRes.y / 2);
  }

  if (cameraMovement.forward)
  {
    camera.forward(speed);
    render.cameraChanged = true;
  }
  if (cameraMovement.left)
  {
    camera.leftRight(speed);
    render.cameraChanged = true;
  }
  if (cameraMovement.backward)
  {
    camera.forward(-speed);
    render.cameraChanged = true;
  }
  if (cameraMovement.right)
  {
    camera.leftRight(-speed);
    render.cameraChanged = true;
  }
  if (cameraMovement.up)
  {
    camera.upDown(-speed);
    render.cameraChanged = true;
  }
  if (cameraMovement.down)
  {
    camera.upDown(speed);
    render.cameraChanged = true;
  }
  if (cameraMovement.rollLeft)
  {
    camera.roll(speed * 0.2f);
    render.cameraChanged = true;
  }
  if (cameraMovement.rollRight)
  {
    camera.roll(-speed * 0.2f);
    render.cameraChanged = true;
  }
  
  glm::vec3 rotation = camera.back();


  

  double angleX = acos(dot(rotation, glm::vec3({ 1, 0, 0 }) / length(rotation)));
  double angleY = acos(dot(rotation, glm::vec3({ 0, 1, 0 }) / length(rotation)));

  glm::vec3 pointOnSphere = { 0,0,0 };
  
  pointOnSphere.x = camera.objectRadius * cos(angleX) * sin(angleY);
  pointOnSphere.z = camera.objectRadius * sin(angleX) * sin(angleY);
  pointOnSphere.y = camera.objectRadius * cos(angleY);

  camera.eye_point = pointOnSphere;

}
