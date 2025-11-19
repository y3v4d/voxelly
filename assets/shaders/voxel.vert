#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in float aTexIndex;
layout(location = 2) in vec3 aNormal;

out vec2 TexCoord;
out vec3 Normal;

const float textureSize = 1.0;
const vec2 atlasSize = vec2(256.0, 256.0);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    float textureWidth = textureSize * atlasSize.x;
    float textureHeight = textureSize * atlasSize.y;

    int row = int(aTexIndex) / int(atlasSize.x);
    int col = int(aTexIndex) % int(atlasSize.x);

    float texX = (float(col) + 0.5) * textureSize;
    float texY = (float(row) + 0.5) * textureSize;

    TexCoord = vec2(texX / textureWidth, texY / textureHeight);
    Normal = aNormal;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}