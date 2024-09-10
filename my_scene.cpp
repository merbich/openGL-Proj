#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <MyFiles/shader.h>
#include <MyFiles/camera.h>
#include <MyFiles/model.h>
#include <MyFiles/stable_camera.h>
#include <MyFiles/moving_camera.h>
#include <MyFiles/FPPCamera.h>

#include "MyFiles/Functions.cpp"
#include "functions/Bezier.cpp"
#include "functions/ShadersParametrs.cpp"

#include <iostream>
#include <format>

enum Camera_Choise {
    STABLE,
    MOVING,
    FPP,
    FREE
};
enum LightningMode {
    basic,
    gouraud,
    phong
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadMyTexture(const char* path);
void setCameraProps();

// settings
const unsigned int SCR_WIDTH = 900;
const unsigned int SCR_HEIGHT = 750;

// cameras
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
StableCamera stableCamera(
    glm::vec3(0.0f, 5.0f, 10.0f),  // Position
    glm::vec3(0.0f, -0.5f, -1.0f), // Front (look direction)
    glm::vec3(0.0f, 1.0f, 0.0f),   // Up vector
    45.0f,                         // Field of View
    (float)SCR_WIDTH / (float)SCR_HEIGHT, // Aspect Ratio
    0.1f,                          // Near clipping plane
    100.0f                         // Far clipping plane
);
MovingCamera movingCamera(
    glm::vec3(0.0f, 10.0f, 0.0f),  // Position
    glm::vec3(0.0f, -0.5f, -1.0f), // Front (look direction)
    glm::vec3(0.0f, 1.0f, 0.0f),
    45.0f,                         // Field of View
    (float)SCR_WIDTH / (float)SCR_HEIGHT, // Aspect Ratio
    0.1f,                          // Near clipping plane
    100.0f   // Up vector
);
FPPCamera FPPcamera(
    glm::vec3(0.0f, 10.0f, 0.0f),  // Position
    glm::vec3(0.0f, -0.5f, -1.0f), // Front (look direction)
    glm::vec3(0.0f, 1.0f, 0.0f),
    45.0f,                         // Field of View
    (float)SCR_WIDTH / (float)SCR_HEIGHT, // Aspect Ratio
    0.1f,                          // Near clipping plane
    100.0f   // Up vector
);

Camera_Choise choise = FREE;
glm::vec3 cameraPosition = camera.Position;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool isDay = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
LightningMode lightningMode = phong;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// projection and view matrices
glm::mat4 projection = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);

std::vector<glm::vec3> BezierVertices;
std::vector<glm::vec3> BezierNormals;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    // flip loaded texture
    // -------------------
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shader program
    // ------------------------------------
    // PHONG LIGHTNING SHADERS
    // ---------------
    Shader objectShader("./shaders/phong_lightning/shader_vs.txt",
        "./shaders/phong_lightning/shader_fs.txt");
    Shader lightShader("./shaders/phong_lightning/light_shader_vs.txt",
        "./shaders/phong_lightning/light_shader_fs.txt");
    Shader bezierShader("./shaders/bezier/bezier_vs.txt",
        "./shaders/bezier/bezier_fs.txt");
    Shader treeShader("./shaders/tree_model/tree_model_vs.txt",
        "./shaders/tree_model/tree_model_fs.txt");
    // GOURAUD LIGHTNING SHADERS
    // ---------------
    /*Shader objectShader("./shaders/gouraud_lightning/gouraud_vs.txt",
        "./shaders/gouraud_lightning/gouraud_fs.txt");
    Shader lightShader("./shaders/phong_lightning/light_shader_vs.txt",
        "./shaders/phong_lightning/light_shader_fs.txt");*/
    // BASIC LIGHTNING SHADERS
    // ---------------
    /*Shader objectShader("./shaders/basic_lightning/shader_vs.txt",
        "./shaders/basic_lightning/shader_fs.txt");
    Shader lightShader("./shaders/phong_lightning/light_shader_vs.txt",
        "./shaders/phong_lightning/light_shader_fs.txt");*/

    // load models
    // -----------
    Model backpack("../models/backpack/backpack.obj");
    Model car("../recource/models/lancia/lacia_delta_9.obj");
    //Model polyCar("../recource/models/MyCar.obj");
    Model tree("../recource/models/tree/Tree1.obj");
    //Model tree("../models/tree/Tree1/Tree1.obj");
    //Model donut("../recource/models/donutWOplane.obj");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    float planeVertices[] = {
        // positions         // normals    // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f, 0.0f,  1.0f,  0.0f, 2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f, 0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f,  1.0f,  0.0f, 0.0f, 2.0f,

         5.0f, -0.5f,  5.0f, 0.0f, 1.0f,  0.0f, 2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 0.0f, 1.0f,  0.0f, 0.0f, 2.0f,
         5.0f, -0.5f, -5.0f, 0.0f, 1.0f,  0.0f, 2.0f, 2.0f
    };
    // configure the cube's VAO (and VBO)
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // floor plane
    // -----------
    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // bezier surface
    std::vector<GLuint> indices;

    int resolution = 10; // Number of divisions for u and v

    updateBezierSurface(&BezierVertices, &BezierNormals, resolution);

    for (int i = 0; i < resolution; ++i) {
        for (int j = 0; j < resolution; ++j) {
            int start = i * (resolution + 1) + j;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + resolution + 1);
            indices.push_back(start + 1);
            indices.push_back(start + resolution + 2);
            indices.push_back(start + resolution + 1);
        }
    }
    CreateBezierVBO(&BezierVertices, &BezierNormals);
    


    unsigned int BezierVAO, BezierVBO, BezierEBO;
    glGenVertexArrays(1, &BezierVAO);
    glGenBuffers(1, &BezierVBO);
    glGenBuffers(1, &BezierEBO);

    glBindVertexArray(BezierVAO);

    glBindBuffer(GL_ARRAY_BUFFER, BezierVBO);
    glBufferData(GL_ARRAY_BUFFER, BeizerData.size() * sizeof(GLfloat), &BeizerData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BezierEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // loading custom textures
    unsigned int floorTexture = loadMyTexture("../recource/textures/grass.jpg");
    unsigned int sunTexture = loadMyTexture("../recource/textures/sun_texture.jpg");

    SetConstShaderParametrs(objectShader);
    SetConstShaderParametrs(bezierShader);
    SetConstShaderParametrs(treeShader);
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        if(isDay)
            glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
        else
            glClearColor(0.0f, 0.0f, 0.18f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;

        setCameraProps();
        SetFlashLights(camera.Position, stableCamera.Position);

        // bezier surface drawing
        updateBezierControlPoints();
        updateBezierSurface(&BezierVertices, &BezierNormals, resolution);
        CreateBezierVBO(&BezierVertices, &BezierNormals);

        glBindBuffer(GL_ARRAY_BUFFER, BezierVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, BeizerData.size() * sizeof(GLfloat), &BeizerData[0]);

        bezierShader.use();
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-3.0f, 2.0f, -5.0f));
        glm::mat4 scalingMatrix = glm::scale(modelMatrix, glm::vec3 (1.5f, 1.0f, 2.0f));
        bezierShader.setMat4("model", modelMatrix);
        bezierShader.setMat4("projection", projection);
        bezierShader.setMat4("view", view);

        SetShaderParametrs(bezierShader, isDay, camera.Front, stableCamera.Front, &flashlightsPos, camera.Position);

        glBindVertexArray(BezierVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // activate shader when setting uniforms/drawing objects
        objectShader.use();
        glm::mat4 model = moveObject();
        objectShader.setMat4("model", model);
        if (choise == MOVING)
            movingCamera.setFrontVector(GetMovingObjPosition());
        if (choise == FPP)
            FPPcamera.setPositionAndFrontVector(GetMovingObjPosition(), GetMovingObjFrontVector());
       
        SetShaderParametrs(objectShader, isDay, camera.Front, stableCamera.Front, &flashlightsPos, camera.Position);

        // view/projection transformations
        objectShader.setMat4("projection", projection);
        objectShader.setMat4("view", view);

        car.Draw(objectShader);

        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.7f, 0.1f, 1.5f));
        model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
        objectShader.setMat4("model", model);
        backpack.Draw(objectShader);

        treeShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, -1.0f, 0.0f));
        treeShader.setMat4("model", model);
        
        SetShaderParametrs(treeShader, isDay, camera.Front, stableCamera.Front, &flashlightsPos, camera.Position);

        // view/projection transformations
        treeShader.setMat4("projection", projection);
        treeShader.setMat4("view", view);
        // draw trees
        //-----------------------
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        tree.Draw(treeShader);
        glDisable(GL_BLEND);
        //-----------------------

        //donut.Draw(objectShader);

        // draw the floor
        objectShader.use();
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        model = glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 1.0f, 5.0f));
        objectShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        

        // also draw the lamp object
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        lightShader.setVec3("light.ambient", 0.129f, 0.104f, 0.186f);
        lightShader.setVec3("light.diffuse", 0.929f, 0.904f, 0.986f);
        lightShader.setVec3("light.specular", 0.929f, 0.904f, 0.986f);

        glBindVertexArray(lightCubeVAO);
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    /*glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);*/

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// chooses which camera is in use depending on which keybord button is pressed
// ---------------------------------------------------------------------------
void setCameraProps()
{
    // view/projection transformations
    switch (choise)
    {
    case FREE:
    {
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        break;
    }
    case STABLE:
    {
        projection = stableCamera.ProjectionMatrix;
        view = stableCamera.GetViewMatrix();
        break;
    }
    case MOVING:
    {
        projection = movingCamera.ProjectionMatrix;
        view = movingCamera.GetViewMatrix();
        break;
    }
    case FPP:
    {
        projection = FPPcamera.ProjectionMatrix;
        view = FPPcamera.GetViewMatrix();
        break;
    }
    }
    return;
}

void ChangeLightningMode()
{
    switch (lightningMode)
    {
    case basic:
    {
        Shader objectShader("./shaders/basic_lightning / shader_vs.txt",
            "./shaders/basic_lightning/shader_fs.txt");
        Shader lightShader("./shaders/phong_lightning/light_shader_vs.txt",
            "./shaders/phong_lightning/light_shader_fs.txt");
        break;
    }
    case phong:
    {
        Shader objectShader("./shaders/phong_lightning/shader_vs.txt",
            "./shaders/phong_lightning/shader_fs.txt");
        Shader lightShader("./shaders/phong_lightning/light_shader_vs.txt",
            "./shaders/phong_lightning/light_shader_fs.txt");
        break;
    }
    case gouraud:
    {
        Shader objectShader("./shaders/gouraud_lightning/gouraud_vs.txt",
            "./shaders/gouraud_lightning/gouraud_fs.txt");
        Shader lightShader("./shaders/phong_lightning/light_shader_vs.txt",
            "./shaders/phong_lightning/light_shader_fs.txt");
        break;
    }
    }
    
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    // day and night
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
        isDay = false;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        isDay = true;

    // cameras
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        choise = STABLE;
        cameraPosition = stableCamera.Position;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        choise = FREE;
        cameraPosition = camera.Position;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        choise = MOVING;
        cameraPosition = movingCamera.Position;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        choise = FPP;
        cameraPosition = FPPcamera.Position;
    }

    // lightning mode
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
        lightningMode = basic;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        lightningMode = phong;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
        lightningMode = gouraud;
    }

    // camera movement
    if (choise == FREE)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if(choise == FREE)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(choise == FREE)
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadMyTexture(const char* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}