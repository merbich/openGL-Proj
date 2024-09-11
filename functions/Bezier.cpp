#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

std::vector<float> BezierData;
glm::vec3 controlPoints[4][4] = {
    {glm::vec3(-1.5, -1.5, 4.0), glm::vec3(-0.5, -1.5, 2.0), glm::vec3(0.5, -1.5, -1.0), glm::vec3(1.5, -1.5, 2.0)},
    {glm::vec3(-1.5, -0.5, 1.0), glm::vec3(-0.5, -0.5, 3.0), glm::vec3(0.5, -0.5, 0.0), glm::vec3(1.5, -0.5, -1.0)},
    {glm::vec3(-1.5,  0.5, 4.0), glm::vec3(-0.5,  0.5, 0.0), glm::vec3(0.5,  0.5, 3.0), glm::vec3(1.5,  0.5, 4.0)},
    {glm::vec3(-1.5,  1.5, -2.0), glm::vec3(-0.5,  1.5, -2.0), glm::vec3(0.5,  1.5, 0.0), glm::vec3(1.5,  1.5, -1.0)}
};

void BernsteinPolynomials(float* b, float* db, float t) {

    b[0] = pow(1.0 - t, 3);
    b[1] = 3.0 * pow(1.0 - t, 2) * t;
    b[2] = 3.0 * (1.0 - t) * pow(t, 2);
    b[3] = pow(t, 3);

    //derivatives
    db[0] = -3.0 * pow(1.0 - t, 2);
    db[1] = -6.0 * (1.0 - t) * t + 3.0 * pow(1.0 - t, 2);
    db[2] = -3.0 * pow(t, 2) + 6.0 * t * (1.0 - t);
    db[3] = 3.0 * pow(t, 2);
}

std::vector<glm::vec3> bezierSurface(float u, float v) {
    std::vector<glm::vec3> result(2, glm::vec3(0.0f));
    //glm::vec3 result(0.0f);
    float bu[4], dbu[4], bv[4], dbv[4];
    BernsteinPolynomials(bu, dbu, u);
    BernsteinPolynomials(bv, dbv, v);
    glm::vec3 Pu (0.0f), Pv (0.0f);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            //float bu = bernstein(i, 3, u); // Compute Bernstein polynomial for u
            //float bv = bernstein(j, 3, v); // Compute Bernstein polynomial for v
            //result += bu * bv * controlPoints[i][j];
            result[0] += bu[i] * bv[j] * controlPoints[i][j];
            Pu += dbu[i] * bv[j];
            Pv += bu[i] * dbv[j];
            result[1] += glm::normalize(glm::cross(Pu, Pv));
        }
    }
    return result;
}

//float bernstein(int i, int n, float t) {
//    return combination(n, i) * pow(t, i) * pow(1 - t, n - i);
//}
//
//int combination(int n, int i) {
//    int result = 1;
//    for (int j = 1; j <= i; j++) {
//        result *= (n - (i - j));
//        result /= j;
//    }
//    return result;
//}

void updateBezierControlPoints() {
    for (int i = 1; i < 3; ++i) {
        for (int j = 1; j < 3; ++j) {
            controlPoints[i][j].y = 2.0f + sin(glfwGetTime()) * 2.0f;
        }
    }
}

//void updateSurface(std::vector<glm::vec3>& vertices, glm::vec3 controlPoints[4][4], int resolution) {
//    vertices.clear();
//    for (int i = 0; i <= resolution; ++i) {
//        for (int j = 0; j <= resolution; ++j) {
//            float u = i / (float)resolution;
//            float v = j / (float)resolution;
//            vertices.push_back(bezierSurface(u, v));
//        }
//    }
//}


//----------------------------------------

void updateBezierSurface(std::vector<glm::vec3>* BezierVertices, std::vector<glm::vec3>* BezierNormals, int resolution)
{
    (*BezierVertices).clear();
    (*BezierNormals).clear();
    (*BezierVertices).reserve(sizeof(glm::vec3) * resolution * resolution);
    (*BezierNormals).reserve(sizeof(glm::vec3) * resolution * resolution);
    for (int i = 0; i <= resolution; ++i) {
        for (int j = 0; j <= resolution; ++j) {
            float u = i / (float)resolution;
            float v = j / (float)resolution;
            std::vector<glm::vec3> result = bezierSurface(u, v);
            (*BezierVertices).push_back(result[0]);
            (*BezierNormals).push_back(result[1]);
        }
    }
}

void CreateBezierVBO(int resolution, std::vector<glm::vec3>* BezierVertices, std::vector<glm::vec3>* BezierNormals)
{
    BezierData.clear();  // Clear the data if it's being reused
    BezierData.reserve(sizeof(glm::vec3) * 6);  // Reserve space for vertex positions and normals

    for (int i = 0; i < resolution * resolution; ++i) {
        BezierData.push_back((*BezierVertices)[i].x);
        BezierData.push_back((*BezierVertices)[i].y);
        BezierData.push_back((*BezierVertices)[i].z);

        BezierData.push_back((*BezierNormals)[i].x);
        BezierData.push_back((*BezierNormals)[i].y);
        BezierData.push_back((*BezierNormals)[i].z);
    }
}