#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <MyFiles/shader.h>
#include <iostream>
#include <vector>

// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(0.7f,  0.2f,  2.0f),
    glm::vec3(2.3f, 0.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3(0.0f,  0.0f, -3.0f)
};
std::vector<glm::vec3> flashLightsPos;


//uniform vec3 viewPos;
//uniform DirLight dirLight;
//uniform PointLight pointLights[NR_POINT_LIGHTS];
//uniform SpotLight spotLights[NR_FLASH_LIGHTS];
//uniform Material material;

void SetConstShaderParametrs(Shader shader)
{
    shader.use();
	// directional light
    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    // point lights
    for (unsigned int i = 0; i < 4; i++)
    {
        std::string pointLightIndex = std::string("pointLights[") + std::to_string(i) + "].";

        shader.setVec3(pointLightIndex + "position", pointLightPositions[i]);
        shader.setVec3(pointLightIndex + "ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3(pointLightIndex + "diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3(pointLightIndex + "specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat(pointLightIndex + "constant", 1.0f);
        shader.setFloat(pointLightIndex + "linear", 0.09f);
        shader.setFloat(pointLightIndex + "quadratic", 0.032f);
    }

    // spotlights
    for (unsigned int i = 0; i < 2; i++)
    {
        std::string spotLightIndex = std::string("spotLights[") + std::to_string(i) + "].";

        //shader.setVec3(spotLightIndex + "position", pointLightPositions[i]);
        shader.setVec3(spotLightIndex + "ambient", 0.05f, 0.05f, 0.05f);
        shader.setVec3(spotLightIndex + "diffuse", 0.8f, 0.8f, 0.8f);
        shader.setVec3(spotLightIndex + "specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat(spotLightIndex + "constant", 1.0f);
        shader.setFloat(spotLightIndex + "linear", 0.09f);
        shader.setFloat(spotLightIndex + "quadratic", 0.032f);
        shader.setFloat(spotLightIndex + "cutOff", glm::cos(glm::radians(12.5f)));
        shader.setFloat(spotLightIndex + "outerCutOff", glm::cos(glm::radians(15.0f)));
        // + direction
    }

    // other
    shader.setFloat("material.shininess", 32.0f);

}

void SetFlashLights(glm::vec3 cameraPosition, glm::vec3 stableCameraPosition)
{
    flashLightsPos.clear();
    flashLightsPos.reserve(sizeof(glm::vec3) * 2);

    flashLightsPos.push_back(cameraPosition);
    flashLightsPos.push_back(stableCameraPosition);
}

void SetShaderParametrs(Shader shader, bool isDay, glm::vec3 cameraFront,
                        glm::vec3 stableCameraFront, std::vector<glm::vec3>* flashPos,
                        glm::vec3 cameraPosition)
{
    shader.use();
    if (isDay)
    {
        shader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        shader.setVec3("dirLight.diffuse", 0.7f, 0.7f, 0.7f);
        shader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
    }
    else
    {
        shader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
        shader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.2f);
        shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    }

    shader.setVec3("viewPos", cameraPosition);
    shader.setVec3("spotLights[0].position", (*flashPos)[0]);
    shader.setVec3("spotLights[1].position", (*flashPos)[1]);
    shader.setVec3("spotLights[0].direction", cameraFront);
    shader.setVec3("spotLights[1].direction", stableCameraFront);

    shader.setBool("isDay", isDay);
}