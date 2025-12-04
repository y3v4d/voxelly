#vertex
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in vec2 a_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 TexCoord;

void main() {
    TexCoord = a_TexCoords;
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Pos, 1.0);
}

#fragment
uniform vec4 u_BorderColor;

in vec2 TexCoord;
out vec4 FragColor;

void main() {
    float borderWidth = 3.0;

    vec2 d = fwidth(TexCoord);
    vec2 edge = abs(fract(TexCoord - 0.5) - 0.5) / (d * borderWidth);
    float line = min(edge.x, edge.y);
    float alpha = 1.0 - min(line, 1.0);

    if(alpha > 0.0) {
        FragColor = vec4(u_BorderColor.rgb, u_BorderColor.a * alpha);
    } else {
        discard;
    }
}