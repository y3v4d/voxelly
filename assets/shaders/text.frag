#version 330 core

uniform sampler2D texture0;

in vec2 TexCoord;

out vec4 FragColor;

void main() {
    vec4 texColor = texture(texture0, vec2(TexCoord.x, TexCoord.y));
    FragColor = vec4(1.0, 1.0, 1.0, texColor.r);
}