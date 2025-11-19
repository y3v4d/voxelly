#version 330 core

uniform vec4 borderColor;

in vec2 TexCoord;
out vec4 FragColor;

void main() {
    float borderWidth = 3;

    vec2 d = fwidth(TexCoord);
    vec2 edge = abs(fract(TexCoord - 0.5) - 0.5) / (d * borderWidth);
    float line = min(edge.x, edge.y);
    float alpha = 1.0 - min(line, 1.0);

    if(alpha > 0.0) {
        FragColor = vec4(borderColor.rgb, borderColor.a * alpha);
    } else {
        discard;
    }
}