#include "Texture.h"
#include "OpenGL.h"

Texture::Texture(GLuint _ID, int _w, int _h, std::string _name, std::string _path) :
	textureID(_ID), tWidth(_w), tHeight(_h), name(_name), path(_path)
{
	color = White;
}

Texture::Texture(Color _color) : textureID(-1), tWidth(0), tHeight(0), path(""), name("")
{
	color = _color;
}

Texture::~Texture()
{
}
