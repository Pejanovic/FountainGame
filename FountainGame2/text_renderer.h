#pragma once

#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

struct Character {
    GLuint TextureID;
    glm::ivec2 Size;
    glm::ivec2 Bearing;
    GLuint Advance;
};

class TextRenderer {
public:
    std::map<char, Character> Characters;
    GLuint VAO, VBO;
    GLuint shaderProgram;

    void Load(std::string fontPath, GLuint fontSize);
    void RenderText(std::string text, float x, float y, float scale, glm::vec3 color, glm::mat4 projection);
};
