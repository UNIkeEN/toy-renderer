#pragma once

#include "camera.h"

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float aspectRatio) : Camera() {
        setAspectRatio(aspectRatio);
    }

    void update() override {
        glm::vec3 front;
        front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        front.y = sin(glm::radians(mPitch));
        front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mDirection = glm::normalize(front);
        mViewMatrix = glm::lookAt(mPosition, mPosition + mDirection, mUp);
        mProjectionMatrix = glm::perspective(glm::radians(mFOV), mAspectRatio, mNear, mFar);
    }
};
