#pragma once

#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fov, float aspectRatio, float nearClip, float farClip)
        : mFOV(fov), mAspectRatio(aspectRatio), mNearClip(nearClip), mFarClip(farClip) {
        update();
    }

    void update() override {
        mViewMatrix = glm::lookAt(mPosition, mPosition + mDirection, mUp);
        mProjectionMatrix = glm::perspective(glm::radians(mFOV), mAspectRatio, mNearClip, mFarClip);
    }

    void setPosition(const glm::vec3& position) {
        mPosition = position;
        update();
    }

    void setDirection(const glm::vec3& direction) {
        mDirection = direction;
        update();
    }

    void setUp(const glm::vec3& up) {
        mUp = up;
        update();
    }

    void setFOV(float fov) {
        mFOV = fov;
        update();
    }

    void setAspectRatio(float aspectRatio) {
        mAspectRatio = aspectRatio;
        update();
    }

    void setNearClip(float nearClip) {
        mNearClip = nearClip;
        update();
    }

    void setFarClip(float farClip) {
        mFarClip = farClip;
        update();
    }

private:
    float mFOV;
    float mAspectRatio;
    float mNearClip;
    float mFarClip;
};
