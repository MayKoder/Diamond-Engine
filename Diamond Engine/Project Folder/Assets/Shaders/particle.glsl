#ifdef vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in mat4 model_matrix;
layout (location = 5) in vec4 color;

out vec2 textureCoords;
out vec4 col;

uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model_matrix * vec4(aPos, 0.0, 1.0);
	textureCoords = vec2((aPos.x + 1) * 0.5,(aPos.y + 1) * 0.5);

	col = color;
}
#endif

#ifdef fragment
#version 330 core
in vec2 textureCoords;
in vec4 col;

out vec4 fragmentColor;

uniform sampler2D ourTexture;

void main() {
	fragmentColor = texture(ourTexture,textureCoords) * col;
}

#endif














