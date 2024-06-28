#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    virtual ~Camera() = default;

    virtual void update() = 0;

    [[nodiscard]] glm::mat4 getViewMatrix() const { return mViewMatrix; }
    [[nodiscard]] glm::mat4 getProjectionMatrix() const { return mProjectionMatrix; }

    [[nodiscard]] glm::vec3 getPosition() const { return mPosition; }
    [[nodiscard]] glm::vec3 getDirection() const { return mDirection; }
    [[nodiscard]] glm::vec3 getUp() const { return mUp; }
    
    [[nodiscard]] float getYaw() const { return mYaw; }
    void setYaw(const float yaw) { mYaw = yaw; update(); }
    [[nodiscard]] float getPitch() const { return mPitch; }
    void setPitch(const float pitch) { mPitch = pitch; update(); }
    [[nodiscard]] float getFOV() const { return mFOV; }
    void setFOV(const float fov) { mFOV = fov; update(); }
    [[nodiscard]] float getNear() const { return mNear; }
    void setNear(const float near) { mNear = near; update(); }
    [[nodiscard]] float getFar() const { return mFar; }
    void setFar(const float far) { mFar = far; update(); }
    [[nodiscard]] float getAspectRatio() const { return mAspectRatio; }
    void setAspectRatio(const float aspectRatio) { mAspectRatio = aspectRatio; update(); }

    // General camera control functions
    void move(const glm::vec3& offset);
    void move(const glm::vec3& direction, float deltaTime);
    void rotate(float yaw, float pitch);
    void zoom(float offset);
    
protected:
    glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;
    glm::vec3 mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 mDirection = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 mUp = glm::vec3(0.0f, 1.0f, 0.0f);
    float mYaw = -90.0f;
    float mPitch = 0.0f;
    float mFOV = 45.0f;
    float mNear = 0.1f;
    float mFar = 100.0f;
    float mAspectRatio;
};