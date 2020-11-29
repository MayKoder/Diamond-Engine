#include "Re_Texture.h"
#include "OpenGL.h"

#include"IM_TextureImporter.h"
#include"IM_FileSystem.h"

ResourceTexture::ResourceTexture(unsigned int _uid) : Resource(_uid, Resource::Type::TEXTURE), textureID(0), tWidth(0), tHeight(0)
{
	color = White;
}

ResourceTexture::~ResourceTexture()
{
}

//bool ResourceTexture::LoadToMemory()
//{
//	return true;
//}

bool ResourceTexture::LoadToMemory()
{
	LOG(LogType::L_WARNING, "Texture loaded to memory");
	char* buffer = nullptr;
	int size = FileSystem::LoadToBuffer(GetLibraryPath(), &buffer);

	textureID = TextureImporter::LoadToMemory(buffer, size, &tWidth, &tHeight);

	delete[] buffer;
	buffer = nullptr;

	return true;
}

bool ResourceTexture::UnloadFromMemory()
{
	if (textureID != 0)
		glDeleteTextures(1, &textureID);

	textureID = 0;
	tWidth = 0;
	tHeight = 0;

	return true;
}
