#include "Texture.h"
#include "OpenGL.h"

Texture::Texture(GLuint _ID, int _w, int _h, const char* _name, const char* _path) :
	textureID(_ID), tWidth(_w), tHeight(_h), name(_name), path(_path)
{
}

Texture::~Texture()
{
}
