#include <pch.h>
#include "Camera.h"

//const float PI = 4.0f*atanf(1.0f);

Camera::Camera(void)
{
  eye_point = glm::vec4(0,0,0,0);

  up_vector = glm::vec4(0,1,0,1);
  right_vector = glm::vec4(1,0,0,1);
  back_vector = glm::vec4(0,0,-1,1);
  //90* fov
  //Pi/2 radians
  //aspect ratio = 1
  near = 1.f;
  far = 10;
  distance = near * 2;
  width = (tanf(PI/4.0f) / distance) * (distance * 2);
  height = width;

  //W of N = (W/D) N
  //same with height
}

Camera::Camera(const glm::vec3& E, const glm::vec3& look, const glm::vec3& rel,
       float fov, float aspect, float N, float F)
{
  /*    
  glm::vec4 eye_point;
  glm::vec4 right_vector, up_vector, back_vector;
  float width, height, distance, near, far;

  N(back glm::vec4) = -I/mag(I)
  U(right glm::vec4) = IxR / mag 
  V(up glm::vec4) = NxU
  */
  eye_point = E;
  
  glm::vec3 newLook = look;

  back_vector = (-1.0f / glm::length(newLook)) * newLook;

  right_vector = ((1.0f / glm::length(cross(look, rel))) * cross(look, rel) );
  up_vector = cross(back_vector, right_vector);

  near = N;
  far = F;
  //decided to place the viewport location as near place * 2 distance 
  distance = abs(N * 2.0f);

  //float nearWidth = /(2 * N);
  //Wn = (W/D)N
  //Wn/n * D = W
  
  //tan(1/x theta) = W/2D
  //aspect = W/H
  
  width = (distance * 2.0f) * tanf(fov/2.0f);
  height = width/aspect;

}

Camera& Camera::forward(float distance)
{
  //C <- C - dn
  eye_point = eye_point - (distance * back_vector);
  return *this;
}

Camera& Camera::leftRight(float distance)
{
  //C <- C - dn
  eye_point = eye_point - (distance * right_vector);
  return *this;
}

Camera& Camera::upDown(float distance)
{
  //C <- C - dn
  eye_point = eye_point - (distance * up_vector);
  return *this;
}

Camera& Camera::zoom(float factor)
{
  zoomScale *= factor;
  //zoom W <- BW H <-BH
  width *= factor;
  height *= factor;
  return *this;
}

void Camera::recalcVectors()
{
  back_vector.x = cos(glm::radians(yawAngle)) * cos(glm::radians(pitchAngle));
  back_vector.y = sin(glm::radians(pitchAngle));
  back_vector.z = sin(glm::radians(yawAngle)) * cos(glm::radians(pitchAngle));
  //mainCamera.cameraRight
  //mainCamera.cameraUp

  back_vector = glm::normalize(-back_vector);
  right_vector = glm::normalize(glm::cross(up_vector, back_vector));
  //up_vector = rotate(glm::radians(90.0f), -right_vector) * vec4(back_vector, 0);

}

Camera& Camera::yaw(float angle)
{
  yawAngle += angle;
  
  recalcVectors();
  return *this;

}

Camera& Camera::pitch(float angle)
{


  pitchAngle += angle;

  recalcVectors();
  return *this;
}

void Camera::ResetRoll()
{
  //invert current roll
  right_vector = rotate(-rollAngle, glm::vec3(back_vector)) * glm::vec4(right_vector, 1);
  //up_vector(θback_vector)up_vector
  up_vector = rotate(-rollAngle, glm::vec3(back_vector)) * glm::vec4(up_vector, 1);
  //set roll to 0
  rollAngle = 0.0f;
}

Camera& Camera::roll(float angle)
{
  rollAngle += angle;
  //right_vector←R(θback_vector)right_vector 
  right_vector = rotate(angle, glm::vec3(back_vector)) * glm::vec4(right_vector,1);
  //up_vector(θback_vector)up_vector
  up_vector = rotate(angle, glm::vec3(back_vector)) * glm::vec4(up_vector,1);
  return *this;
}

glm::vec3 Camera::eye(void) const
{
  return eye_point;
}

glm::vec3 Camera::right(void) const
{
  return right_vector;
}

glm::vec3 Camera::up(void) const
{
  return up_vector;
}


glm::vec3 Camera::back(void) const
{
  return back_vector;
}

glm::vec3 Camera::viewportGeometry(void) const
{
  return glm::vec3(width, height, distance);
}


float Camera::nearDistance(void) const
{
  return near;
}


float Camera::farDistance(void) const
{
  return far;
}