#version 330 core

in vec3 worldPoint;
out vec4 FragColor;

void main() {
    vec2 point2D = worldPoint.xz * 1.0;
    float lineWidth = 2.0;

    vec2 d = fwidth(point2D);
    vec2 grid = abs(fract(point2D - 0.5) - 0.5) / (d * lineWidth);
    float line = min(grid.x, grid.y);
    float alpha = 1.0 - min(line, 1.0);

    vec4 color = vec4(0.2, 0.2, 0.2, alpha * 0.5);

    if(worldPoint.x > -d.x && worldPoint.x < d.x) {
        color.b = 1.0;
    } else if(worldPoint.z > -d.y && worldPoint.z < d.y) {
        color.r = 1.0;
    } else {
        color.a *= 0.2;
    }

    float near = 0.1;
    float far = 100.0;
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    float fogFactor = smoothstep(near, far, depth);
    color.a *= (1.0 - fogFactor);

    FragColor = color;
}