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
    mYaw += yaw;
    mPitch += pitch;

    if (mPitch > 89.0f)
        mPitch = 89.0f;
    if (mPitch < -89.0f)
        mPitch = -89.0f;

    update();
}

void Camera::zoom(float offset) {
    mFOV -= offset;
    if (mFOV < 1.0f)
        mFOV = 1.0f;
    if (mFOV > 45.0f)
        mFOV = 45.0f;
    update();
}
