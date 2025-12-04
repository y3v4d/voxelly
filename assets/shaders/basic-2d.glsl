#vertex
layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_MVP;

out vec2 TexCoord;
out vec4 Color;

void main() {
    TexCoord = a_TexCoord;
    Color = a_Color;
    
    gl_Position = u_MVP * vec4(a_Pos.x, a_Pos.y, -100.0, 1.0);
}

#fragment
uniform sampler2D u_Texture0;
uniform vec4 u_Color;

uniform bool u_UseTexture;
uniform bool u_UseColor;

in vec2 TexCoord;
in vec4 Color;

out vec4 FragColor;

void main() {
    vec4 final = vec4(1.0, 1.0, 1.0, 1.0);
    
    if(u_UseTexture) {
        final = texture(u_Texture0, TexCoord);
    }

    if (u_UseColor) {
        final *= u_Color;
    }

    FragColor = final;
}