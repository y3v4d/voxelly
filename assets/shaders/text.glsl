#vertex
layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TexCoord;

out vec2 TexCoord;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    TexCoord = a_TexCoord;
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Pos.x, a_Pos.y, -100.0, 1.0);
}

#fragment
uniform sampler2D u_Texture0;

in vec2 TexCoord;
out vec4 FragColor;

void main() {
    vec4 final = texture(u_Texture0, TexCoord);
    FragColor = vec4(1.0, 1.0, 1.0, final.r);
}