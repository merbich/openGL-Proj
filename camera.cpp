#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <MyFiles/shader.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

using namespace std;
using namespace glm;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float opasity = 0.5f;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hello", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }



    Shader ourShader("./shaders/shader_vs.txt",
        "./shaders/shader_fs.txt");

    float vertices[] = {
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
       -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
       -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };


    int width, height, nrChannels, width_cat, height_cat, nrChannels_cat;

    unsigned int VBO, VAO, EBO, texture, textureCat;
    glGenTextures(1, &texture);
    glGenTextures(1, &textureCat);
    glBindTexture(GL_TEXTURE_2D, texture);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    stbi_set_flip_vertically_on_load(true);
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on currently bound texture)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load("../recource/textures/pink-stone-texture-paint.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    glBindTexture(GL_TEXTURE_2D, textureCat);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data_cat = stbi_load("../recource/textures/cat-face.png", &width_cat, &height_cat, &nrChannels_cat, 0);
    if (data_cat)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_cat, height_cat, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_cat);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    stbi_image_free(data);
    stbi_image_free(data_cat);

    ourShader.use();
    ourShader.setInt("ourTexture", 0);
    ourShader.setInt("ourTextureCat", 1);


    mat4 projection = mat4(1.0f);
    projection = perspective(radians(45.0f), ((float)(SCR_WIDTH) / (float)(SCR_HEIGHT)), 0.1f, 100.0f);

    //// camera
    //vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
    //vec3 cameraTarget = vec3(0.0f, 0.0f, 0.0f);
    //vec3 cameraDirection = normalize(cameraPos - cameraTarget);
    //vec3 up = vec3(0.0f, 1.0f, 0.0f);
    //vec3 cameraRight = normalize(cross(up, cameraDirection));
    //vec3 cameraUp = cross(cameraDirection, cameraRight);

    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    glEnable(GL_DEPTH_TEST);
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // process input
        processInput(window);

        // render commands
        glClearColor(1.0f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.setFloat("opasity", opasity);
        // view martix
        mat4 view = mat4(1.0f);
        view = translate(view, vec3(0.0f, 0.0f, -7.0f));
        /*view = rotate(view, (float)glfwGetTime() * radians(50.0f), vec3(0.5f, 1.0f, 0.0f));*/
        int viewLoc = glGetUniformLocation(ourShader.ID, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, value_ptr(view));
        // projection martix
        int projectionLoc = glGetUniformLocation(ourShader.ID, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, value_ptr(projection));
        ourShader.use();

        // activate program
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureCat);


        glBindVertexArray(VAO);
        for (int i = 0; i < 10; i++)
        {
            mat4 model = mat4(1.0f);
            model = translate(model, cubePositions[i]);
            if (i % 3 == 0)
            {
                model = rotate(model, (float)glfwGetTime() * radians(20.0f * i), vec3(0.5f, 1.0f, 0.0f));
            }
            else
            {
                model = rotate(model, radians(20.0f * i), vec3(0.5f, 1.0f, 0.0f));
            }
            int modelLoc = glGetUniformLocation(ourShader.ID, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        opasity += 0.001f;
        if (opasity > 1.0f) opasity = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        opasity -= 0.001f;
        if (opasity < 0.0f) opasity = 0.0f;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}