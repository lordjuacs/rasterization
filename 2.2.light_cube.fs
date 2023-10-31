#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Texture coordinates passed from the vertex shader

uniform sampler2D sunTexture;

void main()
{
    FragColor = texture(sunTexture, TexCoords);
}
