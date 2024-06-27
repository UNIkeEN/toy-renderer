#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    virtual ~Camera() = default;

    virtual void update() = 0;
    virtual const glm::mat4& getViewMatrix() const = 0;
    virtual const glm::mat4& getProjectionMatrix() const = 0;

protected:
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;
};
