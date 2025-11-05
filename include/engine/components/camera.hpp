#pragma once

#include "engine/core/core.hpp"
#include "engine/components/transform.hpp"

enum class ProjectionType {
    ORTHOGRAPHIC,
    PERSPECTIVE
};

struct OrthographicSettings {
    float left;
    float right;
    float bottom;
    float top;
};

struct PerspectiveSettings {
    float fov;
    float aspectRatio;
};

class Camera : public Transform {
public:
    Camera(float left, float right, float bottom, float top, float near, float far);
    Camera(float fov, float aspectRatio, float near, float far);
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;

    void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
    void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

    ProjectionType projectionType;

    OrthographicSettings orthoSettings;
    PerspectiveSettings perspSettings;

    float near;
    float far;

    glm::mat4 projectionMatrix;
};