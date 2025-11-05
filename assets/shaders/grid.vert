#version 330 core

vec3 plane_vertices[4] = vec3[4](
    vec3(-1.0, 0.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 0.0, -1.0),
    vec3(-1.0, 0.0, -1.0)
);

int plane_indices[6] = int[6](
    0, 1, 2,
    2, 3, 0
);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 worldPoint;

void main() {
    vec3 pos = plane_vertices[plane_indices[gl_VertexID]] * 1000.0;
    vec4 projected = model * vec4(pos, 1.0);

    worldPoint = projected.xyz;
    gl_Position = projection * view * projected;
}