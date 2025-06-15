#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
uniform sampler2D texture1;
uniform bool useCustomColor;
uniform vec4 customColor;

void main()
{
    if (useCustomColor)
        FragColor = customColor;
    else
        FragColor = texture(texture1, TexCoord);
}
