#include "RE_Material.h"

ResourceMaterial::ResourceMaterial(unsigned int _uid) : Resource(_uid, Resource::Type::TEXTURE), diffuseTexture(nullptr), normalMap(nullptr)
{}

ResourceMaterial::~ResourceMaterial()
{}


