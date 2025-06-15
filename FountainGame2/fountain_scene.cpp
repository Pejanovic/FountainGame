#include "fountain_scene.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <GLFW/glfw3.h>
#include <vector>

// cubeVAO definition (for reuse)
unsigned int cubeVAO = 0, cubeVBO = 0;
static std::vector<glm::vec3> splashPositions;
static std::vector<glm::vec3> splashVelocities;
static std::vector<float> splashRotations;


void initCube() {
    if (cubeVAO != 0) return;
    float vertices[] = {
        // pos           // uv
       -0.5f, -0.5f, -0.5f, 0, 0,  0.5f, -0.5f, -0.5f, 1, 0,  0.5f,  0.5f, -0.5f, 1, 1,
        0.5f,  0.5f, -0.5f, 1, 1, -0.5f,  0.5f, -0.5f, 0, 1, -0.5f, -0.5f, -0.5f, 0, 0,
       -0.5f, -0.5f,  0.5f, 0, 0,  0.5f, -0.5f,  0.5f, 1, 0,  0.5f,  0.5f,  0.5f, 1, 1,
        0.5f,  0.5f,  0.5f, 1, 1, -0.5f,  0.5f,  0.5f, 0, 1, -0.5f, -0.5f,  0.5f, 0, 0,
       -0.5f,  0.5f,  0.5f, 1, 0, -0.5f,  0.5f, -0.5f, 1, 1, -0.5f, -0.5f, -0.5f, 0, 1,
       -0.5f, -0.5f, -0.5f, 0, 1, -0.5f, -0.5f,  0.5f, 0, 0, -0.5f,  0.5f,  0.5f, 1, 0,
        0.5f,  0.5f,  0.5f, 1, 0,  0.5f,  0.5f, -0.5f, 1, 1,  0.5f, -0.5f, -0.5f, 0, 1,
        0.5f, -0.5f, -0.5f, 0, 1,  0.5f, -0.5f,  0.5f, 0, 0,  0.5f,  0.5f,  0.5f, 1, 0,
       -0.5f, -0.5f, -0.5f, 0, 1,  0.5f, -0.5f, -0.5f, 1, 1,  0.5f, -0.5f,  0.5f, 1, 0,
        0.5f, -0.5f,  0.5f, 1, 0, -0.5f, -0.5f,  0.5f, 0, 0, -0.5f, -0.5f, -0.5f, 0, 1,
       -0.5f,  0.5f, -0.5f, 0, 1,  0.5f,  0.5f, -0.5f, 1, 1,  0.5f,  0.5f,  0.5f, 1, 0,
        0.5f,  0.5f,  0.5f, 1, 0, -0.5f,  0.5f,  0.5f, 0, 0, -0.5f,  0.5f, -0.5f, 0, 1
    };

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void drawCube(unsigned int shaderProgram, const glm::mat4& model) {
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void drawPyramid(unsigned int shaderProgram, const glm::mat4& model) {
    float vertices[] = {
        // base
        -0.5f, 0.0f, -0.5f,   0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   1.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   1.0f, 1.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f,
        // tip
         0.0f, 1.0f,  0.0f,   0.5f, 0.5f
    };
    unsigned int indices[] = {
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4
    };

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void drawFountain(unsigned int sceneShader, unsigned int waterShader,
    unsigned int stoneTexture, unsigned int waterTexture,
    const glm::mat4& view, const glm::mat4& projection) {
    float thickness = 0.2f;
    float wallHeight = 1.0f;
    float baseSize = 6.0f;
    float topSize = 3.0f;

    auto drawWallCube = [&](glm::vec3 pos, glm::vec3 scale, unsigned int tex, unsigned int program) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, scale);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUseProgram(program);
        glUniform1i(glGetUniformLocation(program, "texture1"), 0);
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        drawCube(program, model);
        };

    // donji bazen
    drawWallCube({ 0, thickness / 2.0f, 0 }, { baseSize, thickness, baseSize }, stoneTexture, sceneShader);
    float baseY = thickness + wallHeight / 2.0f;
    drawWallCube({ -baseSize / 2 + thickness / 2, baseY, 0 }, { thickness, wallHeight, baseSize }, stoneTexture, sceneShader);
    drawWallCube({ baseSize / 2 - thickness / 2, baseY, 0 }, { thickness, wallHeight, baseSize }, stoneTexture, sceneShader);
    drawWallCube({ 0, baseY, -baseSize / 2 + thickness / 2 }, { baseSize, wallHeight, thickness }, stoneTexture, sceneShader);
    drawWallCube({ 0, baseY, baseSize / 2 - thickness / 2 }, { baseSize, wallHeight, thickness }, stoneTexture, sceneShader);

    // gornji bazen
    float topY = baseY + wallHeight / 2.0f + thickness - 0.7f;
    drawWallCube({ 0, topY, 0 }, { topSize, thickness, topSize }, stoneTexture, sceneShader);
    float wallY = topY + thickness + wallHeight / 2.0f;
    float adjustedWallHeight = wallHeight + 1.0f; 
    drawWallCube({ -topSize / 2 + thickness / 2, wallY - 0.05f - 0.15f, 0 }, { thickness, adjustedWallHeight, topSize }, stoneTexture, sceneShader);
    drawWallCube({ topSize / 2 - thickness / 2, wallY - 0.05f - 0.15f, 0 }, { thickness, adjustedWallHeight, topSize }, stoneTexture, sceneShader);
    drawWallCube({ 0, wallY - 0.05f - 0.15f, -topSize / 2 + thickness / 2 }, { topSize, adjustedWallHeight, thickness }, stoneTexture, sceneShader);
    drawWallCube({ 0, wallY - 0.05f - 0.15f, topSize / 2 - thickness / 2 }, { topSize, adjustedWallHeight, thickness }, stoneTexture, sceneShader);

    float stubHeight = wallY + wallHeight - baseY + 0.5;
    glm::mat4 pillar = glm::translate(glm::mat4(1.0f), glm::vec3(0, baseY + stubHeight / 2.0f, 0));
    pillar = glm::scale(pillar, glm::vec3(1.5f, stubHeight, 1.5f));
    drawCube(sceneShader, pillar);

    auto drawWater = [&](glm::vec3 pos, glm::vec3 scale) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, scale);
        glUseProgram(waterShader);
        glUniform1f(glGetUniformLocation(waterShader, "time"), glfwGetTime());
        glUniformMatrix4fv(glGetUniformLocation(waterShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(waterShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        drawCube(waterShader, model);
        };

    glDisable(GL_CULL_FACE); 
    glEnable(GL_BLEND); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    drawWater({ 0, baseY + wallHeight / 2.0f - 0.5f, 0 }, { baseSize - 0.4f, 0.6f, baseSize - 0.4f });
    drawWater({ 0, wallY + wallHeight / 2.0f - 0.1f, 0 }, { topSize - 0.3f, 0.6f, topSize - 0.3f });

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    if (splashPositions.empty()) {
        for (int i = 0; i < 900; ++i) {
            float angle = glm::radians((float)(rand() % 360));
            float speed = 0.9f + (rand() % 100) / 300.0f; 
            float vertical = 2.5f + (rand() % 100) / 300.0f;
            glm::vec3 velocity = glm::vec3(cos(angle) * speed, vertical, sin(angle) * speed);
            splashPositions.push_back(glm::vec3(0, wallY + wallHeight + 1.2f, 0));
            splashVelocities.push_back(velocity);
            splashRotations.push_back((rand() % 360));
        }
    }

    for (size_t i = 0; i < splashPositions.size(); ++i) {
        splashVelocities[i].y -= 9.81f * 0.04f;
        splashPositions[i] += splashVelocities[i] * 0.08f;
        splashRotations[i] += 2.0f;

        if (splashPositions[i].y < baseY + 0.3f) {
            splashPositions[i] = glm::vec3(0, wallY + wallHeight + 1.2f, 0);
            splashVelocities[i].y = 3.5f + (rand() % 100) / 50.0f;
            splashRotations[i] = 0;
        }

        glm::mat4 splashModel = glm::translate(glm::mat4(1.0f), splashPositions[i]);
        splashModel = glm::rotate(splashModel, glm::radians(splashRotations[i]), glm::vec3(1.0f, 1.0f, 0.5f));
        splashModel = glm::scale(splashModel, glm::vec3(0.04f));

        glUseProgram(sceneShader);
        glUniformMatrix4fv(glGetUniformLocation(sceneShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(sceneShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform1i(glGetUniformLocation(sceneShader, "useCustomColor"), 1);
        glUniform4f(glGetUniformLocation(sceneShader, "customColor"), 0.2, 0.6, 0.9, 0.35);

        drawCube(sceneShader, splashModel);

        glUniform1i(glGetUniformLocation(sceneShader, "useCustomColor"), 0);

    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, wallY + wallHeight + 0.5f, 0));
    model = glm::scale(model, glm::vec3(1.5f));
    glDisable(GL_CULL_FACE);
    drawPyramid(sceneShader, model);
    glEnable(GL_CULL_FACE);

}

