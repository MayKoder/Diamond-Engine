#include "Texture.h"
#include "OpenGL.h"

Texture::Texture(GLuint _ID, int _w, int _h, std::string _name, std::string _path) :
	textureID(_ID), tWidth(_w), tHeight(_h), name(_name), path(_path)
{
}

Texture::~Texture()
{
}
