#pragma once

#include "DEResource.h"

class ResourceTexture;

class ResourceMaterial : public Resource {
public:
	ResourceMaterial(unsigned int _uid);
	~ResourceMaterial();

public:
	ResourceTexture* diffuseTexture;
	ResourceTexture* normalMap;

};
