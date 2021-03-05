#ifdef vertex
#version 330 core

const int MAX_JOINTS = 100;
const int MAX_WEIGHTS = 4;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normals;
layout (location = 3) in vec3 tangents;
layout (location = 4) in vec4 boneIDs;
layout (location = 5) in vec4 weights;
layout (location = 6) in vec3 colors;

out vec3 influenceColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 fPosition;
out vec3 vertexColor;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

uniform float time;

uniform mat4 jointTransforms[MAX_JOINTS];

void main()
{
	vec4 totalPosition = vec4(0.0);
	
	for(int i= 0; i < MAX_WEIGHTS; i++){
	
	  if(boneIDs[i] == -1 || weights[i] == 0)
	   	continue;
	   	
	  if(boneIDs[i] >= MAX_JOINTS)
	  {
	    totalPosition = vec4(position, 1.0f);
	    break;
	  }
	  
	  vec4 localPosition = jointTransforms[int(boneIDs[i])] * vec4(position, 1.0f);
	  totalPosition += localPosition * weights[i];
	  vec3 localNormal = mat3(jointTransforms[int(boneIDs[i])]) * normals;
	
	}
	
	mat4 viewModel = view * model_matrix;
	gl_Position = projection * viewModel * totalPosition;
	TexCoord = texCoord;
	influenceColor = vec3(weights.x, weights.y, weights.z);
	//ourColor = vec3(boneIDs.x / 30, boneIDs.y / 30, boneIDs.z / 30);
	vertexColor = colors; 
}
#endif

#ifdef fragment
#version 330 core
in vec3 influenceColor;
in vec2 TexCoord;
in vec3 vertexColor; 

out vec4 color;
uniform sampler2D diffuseTexture;

void main()
{
	vec3 textureColor = texture(diffuseTexture, TexCoord).rgb;
 	color = vec4(textureColor, 1.0);
 	//color = vec4(influenceColor, 1.0);
 	//color = vec4(vertexColor.x, vertexColor.y, vertexColor.z, 1.0);
}
#endif






































































