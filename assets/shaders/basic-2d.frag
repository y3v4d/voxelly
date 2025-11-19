#version 330 core

uniform sampler2D texture0;
uniform bool useColor;

in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(texture0, TexCoord);
    if (useColor) {
        texColor *= Color;
    }

    FragColor = texColor;
}