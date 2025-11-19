#version 330 core

in vec3 worldPoint;
out vec4 FragColor;

bool isLine(float coord, float borderWidth, float d) {
    return coord > -borderWidth * 0.5 * d && coord < borderWidth * 0.5 * d;
}

uniform vec4 borderColor;

void main() {
    vec2 point2D = worldPoint.xz * 1.0;
    float borderWidth = 3;
    float lineWidthX = 1.0;
    float lineWidthZ = 1.0;

    vec2 d = fwidth(point2D);
    vec4 fillColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 gridColor = borderColor;

    if(isLine(worldPoint.x, borderWidth, d.x)) {
        lineWidthX = borderWidth;
        gridColor.rgba = vec4(1.0, 0.3, 0.3, 0.8);
    } 

    if(isLine(worldPoint.z, borderWidth, d.y)) {
        lineWidthZ = borderWidth;
        gridColor.rgba = vec4(0.3, 0.3, 1.0, 0.8);
    }

    if(isLine(worldPoint.x - floor(worldPoint.x / 8.0) * 8.0, borderWidth, d.x)) {
        lineWidthX = borderWidth / 2;
        gridColor = vec4(0.2, 0.2, 0.2, 0.4);
    }

    if(isLine(worldPoint.z - floor(worldPoint.z / 8.0) * 8.0, borderWidth, d.y)) {
        lineWidthZ = borderWidth / 2;
        gridColor = vec4(0.2, 0.2, 0.2, 0.4);
    }

    vec2 grid = abs(fract(point2D - 0.5) - 0.5) / (d * vec2(lineWidthX, lineWidthZ));
    float line = min(grid.x, grid.y);
    float alpha = 1.0 - min(line, 1.0);

    if(worldPoint.x >= 0.0 && worldPoint.x <= 16.0 && worldPoint.z >= 0.0 && worldPoint.z <= 16.0) {
        fillColor = vec4(1.0, 1.0, 1.0, 0.2);
    }

    vec4 color = mix(fillColor, gridColor, alpha);

    float near = 0.1;
    float far = 100.0;
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = smoothstep(near, far, depth);
    color.a *= (1.0 - fogFactor);

    FragColor = color;
}