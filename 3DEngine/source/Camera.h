#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

inline glm::vec4 cross(const glm::vec4& left, const glm::vec4& right)
{
  glm::vec3 leftv3(left);
  glm::vec3 rightv3(right);

  glm::vec3 ans = cross(leftv3, rightv3);

  return glm::vec4(ans,1);
}

class Camera {
  public:
    Camera(void);
    Camera(const glm::vec3& E, const glm::vec3& look, const glm::vec3& rel,
           float fov, float aspect, float N, float F);

    glm::vec3 eye(void) const;  //camera pos
    glm::vec3 right(void) const;
    glm::vec3 up(void) const;
    glm::vec3 back(void) const;
    glm::vec3 viewportGeometry(void) const;
    float nearDistance(void) const;
    float farDistance(void) const;
    Camera& zoom(float factor);
    Camera& forward(float distance);
    Camera& yaw(float angle);
    Camera& pitch(float angle);
    Camera& roll(float angle);
    Camera& leftRight(float distance);
    Camera& upDown(float distance);
    void ResetRoll();
    glm::vec3 right_vector, up_vector, back_vector;
    float zoomScale = 1;

    glm::vec3 eye_point;
    float objectRadius = 10;

  private:
    void recalcVectors();
    float width, height, distance,
          near, far;

    float pitchAngle = 0;
    float yawAngle = -90;
    float rollAngle = 0;

};


#endif

