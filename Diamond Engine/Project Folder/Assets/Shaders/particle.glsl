#ifdef vertex
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 textureCoords;

uniform mat4 model_matrix;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model_matrix * vec4(aPos, 0.0, 1.0);
	textureCoords = vec2((aPos.x + 1.0) * 0.5,(aPos.y + 1.0) * 0.5);

}
#endif

#ifdef fragment
#version 330 core
in vec2 textureCoords;

out vec4 fragmentColor;

uniform sampler2D ourTexture;

void main() {
	fragmentColor = vec4(1.0, 0.0, 0.0, 1.0);
}

#endif








