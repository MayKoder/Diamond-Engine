#ifdef vertex
#version 330 core
layout (location = 0) in vec2 aPos;
out vec2 textureCoords;

uniform mat4 model_matrix;

void main() {
	gl_Position = model_matrix * vec4(aPos, 0, 1.0);
	textureCoords = vec2((aPos.x + 1.0) * 0.5,(aPos.y + 1.0) * 0.5);

}
#endif

#ifdef fragment
#version 330 core
in vec2 textureCoords;

out vec4 fragmentColor;

uniform sampler2D ourTexture;
uniform float length_used;
uniform int combo_number;

void main() {
	if(textureCoords.x<1-length_used){
		fragmentColor=vec4(0,0,0,0);
		}
	else if(combo_number<10){
		fragmentColor = texture(ourTexture,textureCoords)+vec4(0,0.8,1,1);
	}
	else if(combo_number<25){
		fragmentColor = texture(ourTexture,textureCoords)+vec4(0,1,0,1);
	}
	else if(combo_number<45){
		fragmentColor = texture(ourTexture,textureCoords)+vec4(1,1,0,1);
	}
	else if(combo_number<77){
		fragmentColor = texture(ourTexture,textureCoords)+vec4(0.79,0.28,0.96,1);
	}
	else{
		fragmentColor = texture(ourTexture,textureCoords)+vec4(1,1,1,1);
	}
}

#endif


