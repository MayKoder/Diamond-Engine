#type vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normals;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 fPosition;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
Normal = mat3(transpose(inverse(model_matrix))) * normals;

vec4 pos = model_matrix * vec4(position, 1.0);
fPosition = pos.xyz;

gl_Position = projection * view * model_matrix * vec4(position, 1.0f);

ourColor = color;
TexCoord = texCoord;
}


#type fragment
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;

in vec3 fPosition;
in vec3 Normal;

vec2 blinnPhongDir(vec3 lightDir, float lightInt, float Ka, float Kd, float Ks, float shininess)
{
vec3 s = normalize(lightDir);
vec3 v = normalize(-fPosition);
vec3 n = normalize(Normal);
vec3 h = normalize(v+s);
float diffuse = Ka + Kd * lightInt * max(0.0, dot(n, s));
float spec = Ks * pow(max(0.0, dot(n,h)), shininess);
return vec2(diffuse, spec);
}

void main()
{
color = texture(ourTexture, TexCoord);
}