#pragma once

#include "camera.h"
#include <iostream>

class OrthographicCamera : public Camera {
public:
    explicit OrthographicCamera(float aspectRatio) : Camera() {
        setAspectRatio(aspectRatio);
        // OrthographicCamera::update();
    }

    explicit OrthographicCamera(const Camera& other) : Camera(other) {}

    [[nodiscard]] CAMERA_TYPE getType() const override {
        return CAMERA_TYPE::Orthographic;
    }

    void update() override {
        glm::vec3 front;
        front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        front.y = sin(glm::radians(mPitch));
        front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        mDirection = glm::normalize(front);
        mViewMatrix = glm::lookAt(mPosition, mPosition + mDirection, mUp);

        float orthoScale = 30.0f;
        float left = -orthoScale * mAspectRatio;
        float right = orthoScale * mAspectRatio;
        float bottom = -orthoScale;
        float top = orthoScale;

        mProjectionMatrix = glm::ortho(left, right, bottom, top, mNear, mFar);
    }

    void zoom(float offset) override {}
};
