#ifdef vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 normals;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main()
{
Normal = mat3(transpose(inverse(model_matrix))) * normals;

gl_Position = projection * view * model_matrix * vec4(position, 1.0f);

ourColor = color;
TexCoord = texCoord;
}
#endif

#ifdef fragment
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;
out vec4 color;
uniform sampler2D ourTexture;
void main()
{
color = texture(ourTexture, TexCoord);
}
#endif