#ifdef vertex
#version 330 core
layout (location = 0) in vec4 vertex;
out vec2 texCoords;

uniform mat4 model_matrix;

void main()
{
	gl_Position = model_matrix * vec4(vertex.xy, 0.0, 1.0);
	texCoords = vertex.zw;
}
#endif

#ifdef fragment
#version 330 core
in vec2 texCoords;
out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 altColor;

void main()
{
	vec4 sample = vec4(1.0, 1.0, 1.0, texture(ourTexture, texCoords).r);
	color = vec4(altColor, 1.0) * sample;

}
#endif







