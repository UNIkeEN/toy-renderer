#pragma once

#include "camera.h"

class PerspectiveCamera : public Camera {
public:
    explicit PerspectiveCamera(float aspectRatio) : Camera() {
        setAspectRatio(aspectRatio);
        PerspectiveCamera::update();
    }

    explicit PerspectiveCamera(const Camera& other) : Camera(other) {}

    [[nodiscard]] CAMERA_TYPE getType() const override {
        return CAMERA_TYPE::Perspective;
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

    void zoom(float offset) {}

};
