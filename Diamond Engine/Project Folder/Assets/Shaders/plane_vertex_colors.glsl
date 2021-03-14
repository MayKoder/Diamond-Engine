#ifdef vertex
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 tangents;
layout (location = 4) in vec4 boneIDs;
layout (location = 5) in vec4 weights;
layout (location = 6) in vec3 colors;

out vec3 Normal;
out vec3 fPosition;
out vec3 vertexColor;
out vec3 diffuseColor;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

uniform float time;


uniform vec3 lightPosition;
vec3 lightColor;

void main()
{
	vertexColor = colors;
	
	lightColor = vec3(0.225, 0.150, 0.120);
	vec3 lightDirection = vec3(lightPosition - position);
	diffuseColor = vec3(max(dot(lightDirection, -normals), 0) * lightColor);
	
	gl_Position = projection * view * model_matrix * vec4(position, 1.0);
    
}
#endif

#ifdef fragment
#version 330 core

in vec3 influenceColor;
in vec3 vertexColor; 
in vec3 diffuseColor; 

out vec4 color;

void main()
{
 	color = vec4(vertexColor + diffuseColor, 1.0);
}
#endif










































































