
#ifndef PROJECTION_H
#define PROJECTION_H

#include "Camera.h"
#include <glm/mat4x4.hpp>


glm::mat4 cameraToWorld(const Camera& cam);
glm::mat4 worldToCamera(const Camera& cam);
glm::mat4 cameraToNDC(Camera& cam);
glm::mat4 cameraToWorldSkyBox(const Camera& cam);
glm::mat4 worldToCameraSkyBox(const Camera& cam)
;

#endif

