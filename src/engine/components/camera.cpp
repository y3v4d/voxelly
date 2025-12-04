#include "engine/components/camera.hpp"

Camera::Camera(float fov, float aspectRatio, float nearPlane, float farPlane) {
    setPerspective(fov, aspectRatio, nearPlane, farPlane);
}

Camera::Camera(float left, float right, float bottom, float top, float near, float far) {
    setOrthographic(left, right, bottom, top, near, far);
}

void Camera::setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
    projectionType = ProjectionType::PERSPECTIVE;

    perspSettings.fov = fov;
    perspSettings.aspectRatio = aspectRatio;
    near = nearPlane;
    far = farPlane;
}

void Camera::setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane) {
    projectionType = ProjectionType::ORTHOGRAPHIC;

    orthoSettings.left = left;
    orthoSettings.right = right;
    orthoSettings.bottom = bottom;
    orthoSettings.top = top;
    orthoSettings.zoom = 1.0f;

    near = nearPlane;
    far = farPlane;
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + getForward(), getUp());
}

glm::mat4 Camera::getProjectionMatrix() const {
    if(projectionType == ProjectionType::PERSPECTIVE) {
        return glm::perspective(glm::radians(perspSettings.fov), perspSettings.aspectRatio, near, far);
    } else {
        return glm::ortho(
            orthoSettings.left * orthoSettings.zoom, 
            orthoSettings.right * orthoSettings.zoom, 
            orthoSettings.bottom * orthoSettings.zoom, 
            orthoSettings.top * orthoSettings.zoom, 
            near, far
        );
    }
}

Ray Camera::getRayFromScreenCoords(float screenX, float screenY, float screenWidth, float screenHeight) const {
    auto viewMatrix = getViewMatrix();
    auto projectionMatrix = getProjectionMatrix();

    glm::vec3 nearPlane = glm::unProject(
        glm::vec3(screenX, screenY, 0.0f),
        viewMatrix,
        projectionMatrix,
        glm::vec4(0, 0, screenWidth, screenHeight)
    );

    glm::vec3 farPlane = glm::unProject(
        glm::vec3(screenX, screenY, 1.0f),
        viewMatrix,
        projectionMatrix,
        glm::vec4(0, 0, screenWidth, screenHeight)
    );

    glm::vec3 direction = glm::normalize(farPlane - nearPlane);
    return Ray(nearPlane, direction);
}