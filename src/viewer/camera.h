#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    virtual ~Camera() = default;

    virtual void update() = 0;
    glm::mat4 getViewMatrix() const { return mViewMatrix; }
    glm::mat4 getProjectionMatrix() const { return mProjectionMatrix; }

protected:
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;
    glm::vec3 mPosition;
    glm::vec3 mDirection;
    glm::vec3 mUp;
};
