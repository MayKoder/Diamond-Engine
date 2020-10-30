#pragma once

typedef unsigned int GLuint;

class Texture
{
public:
	Texture(GLuint _ID, int _w, int _h, const char* _name = nullptr, const char* _path = nullptr);
	~Texture();

public:

	const char* path;
	const char* name;

	int tWidth;
	int tHeight;

	GLuint textureID;

};
