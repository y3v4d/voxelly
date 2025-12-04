#vertex
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

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 worldPoint;

void main() {
    vec3 pos = plane_vertices[plane_indices[gl_VertexID]] * 1000.0;
    vec4 projected = u_Model * vec4(pos, 1.0);

    worldPoint = projected.xyz;
    gl_Position = u_Projection * u_View * projected;
}

#fragment
uniform vec4 u_BorderColor;

in vec3 worldPoint;
out vec4 FragColor;

bool isLine(float coord, float borderWidth, float d) {
    return coord > -borderWidth * 0.5 * d && coord < borderWidth * 0.5 * d;
}

void main() {
    vec2 point2D = worldPoint.xz * 1.0;
    float borderWidth = 6.0;
    float lineWidthX = 1.0;
    float lineWidthZ = 1.0;

    vec2 d = fwidth(point2D);
    vec4 fillColor = vec4(1.0, 1.0, 1.0, 0.0);
    vec4 gridColor = vec4(0.2, 0.2, 0.2, 1.0);

    if(worldPoint.x <= 0.0 || worldPoint.z <= 0.0) {
        discard;
    }

    if(isLine(worldPoint.x, borderWidth, d.x)) {
        lineWidthX = borderWidth;
        gridColor.rgba = vec4(1.0, 0.3, 0.3, 0.8);
        fillColor = vec4(gridColor.rgb, 0.0);
    }

    if(isLine(worldPoint.z, borderWidth, d.y)) {
        lineWidthZ = borderWidth;
        gridColor.rgba = vec4(0.3, 0.3, 1.0, 0.8);
        fillColor = vec4(gridColor.rgb, 0.0);
    }

    vec2 grid = abs(fract(point2D - 0.5) - 0.5) / (d * vec2(lineWidthX, lineWidthZ));
    float dist = min(grid.x, grid.y);
    float w = fwidth(dist);
    float line = 1.0 - dist;

    /*if(worldPoint.x >= borderWidth * 0.5 * d.x && worldPoint.x <= 16.0 && worldPoint.z >= borderWidth * 0.5 * d.y && worldPoint.z <= 16.0) {
        //fillColor = vec4(1.0, 1.0, 1.0, 0.2);
        fillColor = vec4(0.0, 0.0, 0.0, 0.0);
    }*/

    vec4 color = mix(fillColor, gridColor, line);

    float near = 0.1;
    float far = 100.0;
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = smoothstep(near, far, depth);
    color.a *= (1.0 - fogFactor);

    FragColor = color;
}