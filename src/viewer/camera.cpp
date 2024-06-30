#include "camera.h"

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
    setYaw(mYaw + yaw);
    setPitch(mPitch + pitch);
    // Omit update() here, setPitch() will call
}

void Camera::zoom(float offset) {
    setFOV(mFOV - offset);
}

void Camera::resetControl() {
    mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    mDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    mYaw = -90.0f;
    mPitch = 0.0f;
    mFOV = 45.0f;
    mNear = 0.1f;
    mFar = 1000.0f;
    update();
}

// Implement of Copy assignment operator
Camera& Camera::operator=(const Camera& other) {
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
}