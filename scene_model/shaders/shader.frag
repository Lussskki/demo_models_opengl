#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1; // Texture sampler
uniform bool useTexture;    // Toggle between color and texture

void main() 
{
    if(useTexture)
        FragColor = texture(texture1, TexCoord);
    else
        FragColor = vec4(ourColor, 1.0);
}
