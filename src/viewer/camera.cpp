#include "camera.h"
#include <iostream>
// Implement of eneral camera control functions

void Camera::move(const glm::vec3& direction, float velocity) {
    mPosition += direction * velocity;
    update();
}

void Camera::move(const glm::vec3& offset) {
    mPosition += offset;
    update();
}

void Camera::rotate(float yaw, float pitch) {
    mYaw += yaw;
    setPitch(mPitch + pitch);
    // Omit update() here, setPitch() will call
}

void Camera::zoom(float offset) {
    setFOV(mFOV - offset);
}

// Implement of Copy assignment operator
Camera& Camera::operator=(const Camera& other) {
    try {
    if (this != &other) {
        mPosition = other.mPosition;
        mDirection = other.mDirection;
        mUp = other.mUp;
        mYaw = other.mYaw;
        mPitch = other.mPitch;
        mFOV = other.mFOV;
        mNear = other.mNear;
        mFar = other.mFar;
        mAspectRatio = other.mAspectRatio;
    }
    return *this;
    } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
}