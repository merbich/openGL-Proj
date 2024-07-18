#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <MyFiles/shader.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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


    
    Shader ourShader("C:/Users/user/Desktop/sem5/GrafikaKomputerowa/openGLProj/GraphProj/shaders/shader_vs.txt",
        "C:/Users/user/Desktop/sem5/GrafikaKomputerowa/openGLProj/GraphProj/shaders/shader_fs.txt");

    float verticesFirst[] = {
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.75f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.25f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    float verticesSecond[] = {
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.75f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.25f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    };

    unsigned int VBOFirst, VAOFirst, VBOSecond, VAOSecond, EBO;
    // generate a buffer with ID 1
    glGenBuffers(1, &VBOFirst);
    glGenBuffers(1, &VBOSecond);
    glGenVertexArrays(1, &VAOFirst);
    glGenVertexArrays(1, &VAOSecond);

    //---FIRST TRIANGLE---
    // bind this buffer to GL_ARRAY_BUFFER
    glBindVertexArray(VAOFirst);
    glBindBuffer(GL_ARRAY_BUFFER, VBOFirst);
    // send vertices data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesFirst), verticesFirst, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // ---SECOND TRIANGLE---
    // bind this buffer to GL_ARRAY_BUFFER
    glBindVertexArray(VAOSecond);
    glBindBuffer(GL_ARRAY_BUFFER, VBOSecond);
    // send vertices data to GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesSecond), verticesSecond, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // process input
        processInput(window);

        // render commands
        glClearColor(1.0f, 0.8f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        ourShader.setFloat("offset", 0.2f);
        // activate program
        glBindVertexArray(VAOFirst);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        /*glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);*/
        glBindVertexArray(VAOSecond);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAOFirst);
    glDeleteBuffers(1, &VBOFirst);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}