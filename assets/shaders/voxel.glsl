#vertex
layout(location = 0) in vec3 a_Pos;
layout(location = 1) in float a_TexIndex;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec2 TexCoord;
out vec3 Normal;

const float textureSize = 1.0;
const vec2 atlasSize = vec2(256.0, 256.0);

void main() {
    float textureWidth = textureSize * atlasSize.x;
    float textureHeight = textureSize * atlasSize.y;

    int row = int(a_TexIndex) / int(atlasSize.x);
    int col = int(a_TexIndex) % int(atlasSize.x);

    float texX = (float(col) + 0.5) * textureSize;
    float texY = (float(row) + 0.5) * textureSize;

    TexCoord = vec2(texX / textureWidth, texY / textureHeight);
    Normal = a_Normal;
    
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Pos, 1.0);
}

#fragment
uniform sampler2D u_Texture0;
uniform bool u_UseTexture;
uniform bool u_UseLight;
uniform vec4 u_ColorTint;
uniform vec3 u_LightDir;

const float ambientStrength = 0.4;

const vec3 lightColor = vec3(1.0, 1.0, 1.0);

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(u_LightDir);
    vec4 texColor = u_UseTexture ? texture(u_Texture0, TexCoord) : vec4(1.0, 1.0, 1.0, 1.0);

    if(u_UseLight) {
        vec3 ambient = ambientStrength * lightColor;
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * lightColor;

        texColor.rgb *= ambient + diffuse;
    }

    texColor *= u_ColorTint;
    FragColor = texColor;
}