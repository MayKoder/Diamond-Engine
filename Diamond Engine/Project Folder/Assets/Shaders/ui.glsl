#ifdef vertex
#version 330 core
in vec2 position;
out vec2 textureCoords;

uniform mat4 model_matrix;

void main() {
	gl_Position = model_matrix * vec4(position, 0.0, 1.0);
	textureCoords = vec2((position.x + 1.0) / 2.0, 1 - (position.y + 1.0)/2.0);

}
#endif

#ifdef fragment
#version 330 core
in vec2 textureCoords;

out vec4 fragmentColor;

uniform sampler2D guiTexture;

void main() {
	fragmentColor = texture(guiTexture,textureCoords);
}

#endif













