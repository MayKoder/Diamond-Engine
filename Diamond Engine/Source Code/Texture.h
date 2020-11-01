#pragma once

#include<string>
typedef unsigned int GLuint;


class Texture
{
public:
	Texture(GLuint _ID, int _w, int _h, std::string _name, std::string _path);
	~Texture();

public:

	std::string name;
	std::string path;

	int tWidth;
	int tHeight;

	GLuint textureID;
};
