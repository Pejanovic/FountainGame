#version 330 core
in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D concreteTexture;

void main() {
    vec4 texColor = texture(concreteTexture, TexCoord);
    if (texColor.a < 0.1)
        discard; // providne delove ne crtaj
    FragColor = texColor;
}