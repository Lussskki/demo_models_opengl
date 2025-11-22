#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform sampler2D texture1;
uniform bool useTexture;

void main()
{
    vec3 norm = normalize(Normal);

    // Ambient
    vec3 ambient = 0.2 * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5 * spec * lightColor;

    vec3 lighting = (ambient + diffuse + specular);

    vec4 texColor = useTexture ? texture(texture1, TexCoord) : vec4(Color, 1.0);

    FragColor = vec4(lighting, 1.0) * texColor;
}
