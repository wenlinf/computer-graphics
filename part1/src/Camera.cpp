#include "Camera.hpp"

#include "glm/gtx/transform.hpp"

#include <iostream>

void Camera::MouseLook(int mouseX, int mouseY){
    float xOffset = m_oldMousePosition.x - mouseX;
    float yOffset = m_oldMousePosition.y - mouseY;
    // set old mouse position
    m_oldMousePosition.x = mouseX;
    m_oldMousePosition.y = mouseY;
    // define sensitivity
    float sensitivity = 0.3f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;
    // update yaw and pitch
    yaw += xOffset;
    pitch += yOffset;
    // add constraints to camera so it doesn't look up or down more than 90.0f
    if(pitch > 89.0f)
        pitch =  89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;
    // update view direction
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    m_viewDirection = glm::normalize(direction);
}

// OPTIONAL TODO: 
//               The camera could really be improved by
//               updating the eye position along the m_viewDirection.
//               Think about how you can do this for a better camera!

void Camera::MoveForward(float speed){
    m_eyePosition += m_viewDirection * speed;
}

void Camera::MoveBackward(float speed){
    m_eyePosition -= m_viewDirection * speed;
}

void Camera::MoveLeft(float speed){
    glm::vec3 right_vector = glm::normalize(glm::cross(m_viewDirection, m_upVector));
    m_eyePosition -= right_vector * speed;
}

void Camera::MoveRight(float speed){
    glm::vec3 right_vector = glm::normalize(glm::cross(m_viewDirection, m_upVector));
    m_eyePosition += right_vector * speed;
}

void Camera::MoveUp(float speed){
    m_eyePosition = m_eyePosition + m_upVector * speed;
}

void Camera::MoveDown(float speed){
    m_eyePosition = m_eyePosition - m_upVector * speed;
}

// Set the position for the camera
void Camera::SetCameraEyePosition(float x, float y, float z){
    m_eyePosition.x = x;
    m_eyePosition.y = y;
    m_eyePosition.z = z;
}

float Camera::GetEyeXPosition(){
    return m_eyePosition.x;
}

float Camera::GetEyeYPosition(){
    return m_eyePosition.y;
}

float Camera::GetEyeZPosition(){
    return m_eyePosition.z;
}

float Camera::GetViewXDirection(){
    return m_viewDirection.x;
}

float Camera::GetViewYDirection(){
    return m_viewDirection.y;
}

float Camera::GetViewZDirection(){
    return m_viewDirection.z;
}


Camera::Camera(){
    std::cout << "(Constructor) Created a Camera!\n";
	// Position us at the origin.
    m_eyePosition = glm::vec3(0.0f,0.0f, 0.0f);
	// Looking down along the z-axis initially.
	// Remember, this is negative because we are looking 'into' the scene.
    m_viewDirection = glm::vec3(0.0f,0.0f, -1.0f);
	// For now--our upVector always points up along the y-axis
    m_upVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::mat4 Camera::GetWorldToViewmatrix() const{
    // Think about the second argument and why that is
    // setup as it is.
    return glm::lookAt( m_eyePosition,
                        m_eyePosition + m_viewDirection,
                        m_upVector);
}
