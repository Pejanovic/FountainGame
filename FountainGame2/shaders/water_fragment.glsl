#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform float time;

void main()
{
    vec4 waterColor = vec4(0.2, 0.6, 0.9, 0.35);
    FragColor = waterColor;
}