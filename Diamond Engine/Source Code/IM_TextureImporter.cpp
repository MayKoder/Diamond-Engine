#include"IM_TextureImporter.h"

#include"IM_FileSystem.h"

#include"Globals.h"

#include "OpenGL.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#include"DEResource.h"
#include"RE_Texture.h"


GLuint TextureImporter::LoadToMemory(char* buffer, int size, int* w, int* h)
{
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG(LogType::L_ERROR, "Image not loaded");
	}

	if (w)
		*w = ilGetInteger(IL_IMAGE_WIDTH);
	if (h)
		*h = ilGetInteger(IL_IMAGE_HEIGHT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint glID = ilutGLBindTexImage();

	//TODO: Generate mipmaps and use best settings
	glBindTexture(GL_TEXTURE_2D, glID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	ilDeleteImages(1, &imageID);
	glBindTexture(GL_TEXTURE_2D, 0);

	return glID;
}

void TextureImporter::SaveDDS(char* buffer, int size, const char* fileName)
{
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG(LogType::L_ERROR, "Image not loaded");
	}

	//TODO: Move this to function
	ILuint _size = 0;
	ILubyte* data = nullptr;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	_size = ilSaveL(IL_DDS, nullptr, 0);
	if (_size > 0) 
	{
		data = new ILubyte[_size];
		ilSaveL(IL_DDS, data, _size);

		std::string path(fileName);
		//path += ".dds";

		FileSystem::Save(path.c_str(), (char*)data, _size, false);

		delete[] data;
		data = nullptr;
	}

	ilDeleteImages(1, &imageID);
}

void TextureImporter::Import(char* buffer, int bSize, Resource* res)
{
	SaveDDS(buffer, bSize, res->GetLibraryPath());
}

/*Take a screenshot*/
void TextureImporter::TakeScreenshot(int frameBuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	ILuint imageID = ilGenImage();
	ilBindImage(imageID);
	ilutGLScreen();
	ilEnable(IL_FILE_OVERWRITE);
	ilSaveImage("Screenshots/Screenshot.png");
	ilDeleteImage(imageID);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
