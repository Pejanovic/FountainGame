#ifndef FOUNTAIN_SCENE_H
#define FOUNTAIN_SCENE_H

#include <glm/glm.hpp>

void initCube();
void drawCube(unsigned int shaderProgram, const glm::mat4& model);
void drawPyramid(unsigned int shaderProgram, const glm::mat4& model);
void drawFountain(unsigned int sceneShader, unsigned int waterShader,
    unsigned int stoneTexture, unsigned int waterTexture,
    const glm::mat4& view, const glm::mat4& projection);

#endif // FOUNTAIN_SCENE_H