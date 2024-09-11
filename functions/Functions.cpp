#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

glm::vec3 movingObjPosition;
glm::vec3 movingFrontVector;

float radiusA = 20.0f;
float radiusB = 15.0f;
float speed = 0.5f;

glm::mat4 moveObject()
{
    // car movement
    glm::vec3 center = glm::vec3(0.0f, 0.0f, 0.0f);

    float time = glfwGetTime();

    float theta = speed * time;

    float x = center.x + radiusA * cos(theta);
    float z = center.z + radiusB * sin(theta);
    movingObjPosition = glm::vec3(x, 0.5f, z);

    float dx = -radiusA * sin(theta);
    float dz = radiusB * cos(theta);
    movingFrontVector = glm::normalize(glm::vec3(dx, 0.0f, dz));

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, movingObjPosition);
    float angle = glm::atan(dx, dz);
    model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));

    return model;
}

glm::vec3 GetMovingObjPosition()
{
    return movingObjPosition;
}

glm::vec3 GetMovingObjFrontVector()
{
    return movingFrontVector;
}