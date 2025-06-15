#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>

#include "shader_utils.h"
#include "fountain_scene.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "text_renderer.h"
#include <vector>

//Text
TextRenderer text;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Kamera
glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 15.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.3f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f, pitch = 0.0f, lastX = 400, lastY = 300;
bool firstMouse = true;
float fov = 45.0f;

// Zvijezde
std::vector<glm::vec3> stars;
const int NUM_STARS = 200;



unsigned int groundVAO, groundVBO;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

template <typename T>
T clamp(T val, T minVal, T maxVal) {
    return std::max(minVal, std::min(val, maxVal));
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) { firstMouse = false; lastX = xpos; lastY = ypos; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;
    float sensitivity = 0.1f;
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;
    pitch = clamp(pitch, -89.0f, 89.0f);
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double, double yoffset) {
    fov -= yoffset;
    fov = clamp(fov, 1.0f, 45.0f);
}

void processInput(GLFWwindow* window, float deltaTime) {
    float speed = 5.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= speed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

unsigned int loadTexture(const char* path) {
    unsigned int texID;
    glGenTextures(1, &texID);
    int w, h, nrChannels;
    unsigned char* data = stbi_load(path, &w, &h, &nrChannels, 0);
    if (data) {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    return texID;
}

void setupGroundPlane() {
    float vertices[] = {
        // pos            // uv
        -25, 0,  25,      0, 0,
         25, 0,  25,     10, 0,
        -25, 0, -25,      0, 10,
         25, 0, -25,     10, 10
    };
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glBindVertexArray(groundVAO);
    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Fountain", nullptr, nullptr);
    if (!window) { std::cerr << "Failed to create GLFW window\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n"; return -1;
    }

    for (int i = 0; i < NUM_STARS; i++) {
        float x = static_cast<float>(rand() % 100 - 50); 
        float y = static_cast<float>(rand() % 50 + 10);
        float z = static_cast<float>(rand() % 100 - 50);
        stars.push_back(glm::vec3(x, y, z));
    }

    GLuint starsVAO, starsVBO;
    glGenVertexArrays(1, &starsVAO);
    glGenBuffers(1, &starsVBO);
    glBindVertexArray(starsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, starsVBO);
    glBufferData(GL_ARRAY_BUFFER, stars.size() * sizeof(glm::vec3), stars.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initCube();
    setupGroundPlane();

    unsigned int sceneShader = createShaderProgram("shaders/scene_vertex.glsl", "shaders/scene_fragment.glsl");
    unsigned int waterShader = createShaderProgram("shaders/water_vertex.glsl", "shaders/water_fragment.glsl");
    unsigned int groundShader = createShaderProgram("shaders/ground_vertex.glsl", "shaders/ground_fragment.glsl");
    unsigned int starsShader = createShaderProgram("shaders/stars_vertex.glsl", "shaders/stars_fragment.glsl");


    unsigned int stoneTex = loadTexture("textures/stone.png");
    unsigned int waterTex = loadTexture("textures/water1.png");
    unsigned int groundTex = loadTexture("textures/ground.png");

    float lastFrame = 0.0f;

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);


    text.Load("ArialUnicode.otf", 48);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window, deltaTime);

        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(fov), 800.f / 600.f, 0.1f, 100.f);

        // Ground
        glUseProgram(groundShader);
        glBindVertexArray(groundVAO);
        glBindTexture(GL_TEXTURE_2D, groundTex);
        glUniform1i(glGetUniformLocation(groundShader, "texture1"), 0);
        glUniformMatrix4fv(glGetUniformLocation(groundShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(groundShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glm::mat4 model = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(groundShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Fountain
        glDisable(GL_CULL_FACE);

        drawFountain(sceneShader, waterShader, stoneTex, waterTex, view, projection);

        glEnable(GL_CULL_FACE);

        glUseProgram(starsShader);
        glUniformMatrix4fv(glGetUniformLocation(starsShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(starsShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glBindVertexArray(starsVAO);
        glDrawArrays(GL_POINTS, 0, NUM_STARS);


        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glm::mat4 textProjection = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);
        text.RenderText("Nikola Pejanovic RA237/2021", 25.0f, 25.0f, 1.0f, glm::vec3(0.1f, 1.0f, 0.1f), textProjection);
        glEnable(GL_DEPTH_TEST); 
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);   
        glFrontFace(GL_CCW);      


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}