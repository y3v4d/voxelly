#version 330 core

float ambientStrength = 0.4;

vec3 lightColor = vec3(1.0, 1.0, 1.0);
vec3 lightDir = normalize(vec3(-1.0, 0.5, 0.2));

uniform sampler2D texture0;
uniform bool useTexture;
uniform bool useLight;
uniform vec4 colorTint;

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

void main() {
    vec4 texColor = useTexture ? texture(texture0, TexCoord) : vec4(1.0, 1.0, 1.0, 1.0);

    if(useLight) {
        vec3 ambient = ambientStrength * lightColor;
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * lightColor;

        texColor.rgb *= ambient + diffuse;
    }

    texColor *= colorTint;
    FragColor = texColor;
}