#include "Camera.hpp"

#include <cmath>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <iostream>

#include "Math.hpp"

namespace Engine {

Camera::Camera() {}

Camera::Camera(glm::vec3 position, glm::vec3 up, glm::vec3 front)
: position(std::move(position)),
  up(std::move(up)),
  front(std::move(front)) {}

Camera::~Camera() {}

glm::mat4 Camera::viewMatrix() const { return glm::lookAt(position, position + front, up); }

glm::mat4 Camera::projectionMatrix() const { return mode == Projection::PERSPECTIVE ? perspective : orthogonal; }

glm::mat4 Camera::orthogonalMatrix() const { return orthogonal; }

glm::vec3 Camera::positionVec() const { return position; }

glm::vec3 Camera::upVec() const { return up; }

glm::vec2 Camera::size() const { return {this->width, this->height}; }

void Camera::setSize(int width, int height) {
    this->width  = width;
    this->height = height;

    setPerspective(fieldOfView, zNear, zFar);
    setOrthogonal(zNear, zFar, zoom);
}

void Camera::setZoom(float zoom) { setOrthogonal(zNear, zFar, zoom); }

void Camera::setPerspective(float fieldOfView, float zNear, float zFar) {
    this->fieldOfView = fieldOfView;
    this->zoom        = 1.0f / (glm::tan(this->fieldOfView / 2.0f));
    this->zNear       = zNear;
    this->zFar        = zFar;
    float aspect      = static_cast<float>(width) / static_cast<float>(height);

    perspective = glm::perspective(fieldOfView, aspect, zNear, zFar);
}

void Camera::setFieldOfView(float fieldOfView) { setPerspective(fieldOfView, zNear, zFar); }

void Camera::setOrthogonal(float zNear, float zFar, float zoom) {
    this->zNear       = zNear;
    this->zFar        = zFar;
    this->zoom        = zoom;
    this->fieldOfView = 2.0f * glm::atan(1.0f / this->zoom);
    float aspect      = static_cast<float>(width) / static_cast<float>(height);

    float distance = glm::length(this->position);

    float widthHalf  = (1.0f / zoom) * distance * aspect;
    float heightHalf = (1.0f / zoom) * distance;
    orthogonal       = glm::ortho(-widthHalf, widthHalf, -heightHalf, heightHalf, zNear, zFar);
}

void Camera::setProjection(Projection mode) { this->mode = mode; }

void Camera::setPosition(glm::vec3 position) { this->position = position; }

void Camera::setRotation(glm::quat rotation) {
    this->rotation = rotation;
    this->front    = glm::normalize((this->rotation * glm::vec3(0.0f, 0.0f, -1.0f)));
}

void Camera::move(const glm::vec3& offset) {
    position += front * offset.z;
    position += up * offset.y;
    position += glm::normalize(glm::cross(front, up)) * offset.x;
}

void Camera::inversePitch() { this->front = glm::reflect(this->front, this->up); }

glm::vec3 Camera::frontVec() const { return front; }

glm::vec3 Camera::rightVec() const { return glm::normalize(glm::cross(front, up)); }

glm::quat Camera::rotationQuat() const { return this->rotation; }

glm::mat4 Camera::worldMatrix() const {
    glm::mat4 model = glm::toMat4(rotation);
    model           = glm::translate(glm::mat4(1.0f), position) * model;
    return model;
}

}  // namespace Engine
