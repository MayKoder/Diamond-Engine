#pragma once

#include"DEResource.h"
#include<string>
#include"Color.h"
typedef unsigned int GLuint;


class ResourceTexture : public Resource
{
public:
	ResourceTexture(unsigned int _uid);
	~ResourceTexture();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

public:
	int tWidth;
	int tHeight;

	GLuint textureID;
	Color color;

};
