#include "IM_FontImporter.h"

#include "Globals.h"

#include "OpenGL.h"

#include "FreeType/include/freetype/freetype.h"
#include "FreeType/include/ft2build.h"
#include FT_FREETYPE_H

#pragma comment (lib, "FreeType/win32/freetype.lib")

Character::Character(unsigned int textureId, unsigned int advanceX, unsigned int advanceY, float sizeX, float sizeY, float bearingX, float bearingY) :
	textureId(textureId)
{
	advance[0] = advanceX;
	advance[1] = advanceY;

	size[0] = sizeX;
	size[1] = sizeY;

	bearing[0] = bearingX;
	bearing[1] = bearingY;
}


Character::~Character()
{
}


FontDictionary::FontDictionary(const char* name, std::map<char, Character>& characterVec) :
	name(name),
	characters(characterVec)
{
}


FontDictionary::~FontDictionary()
{
	characters.clear();
}


void FontDictionary::UnloadCharacterTextures()
{
	int charactersCount = characters.size();

	for (std::map<char, Character>::iterator it = characters.begin(); it != characters.end(); ++it)
	{
		if (it->second.textureId != 0)
			glDeleteTextures(1, &it->second.textureId);

		it->second.textureId = 0;
	}
}


FreeType_Library::FreeType_Library()
{
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
		LOG(LogType::L_ERROR, "Error initializing FreeType");
}

FreeType_Library::~FreeType_Library()
{
	for (std::map<std::string, FontDictionary>::iterator it = fontLibrary.begin(); it != fontLibrary.end(); ++it)
		it->second.UnloadCharacterTextures();

	fontLibrary.clear();
	FT_Done_FreeType(library);
}


void FreeType_Library::ImportNewFont(const char* path, int size)
{
	FT_Face new_face;
	FT_Error error = FT_New_Face(library, path, 0, &new_face);

	if (error)
	{
		LOG(LogType::L_ERROR, "Error while loading font");
	}
	else
	{
		LOG(LogType::L_NORMAL, "The font was loaded correctly");
		FT_Set_Pixel_Sizes(new_face, 0, size);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		InitFontDictionary(new_face, path);
	}
}


void FreeType_Library::InitFontDictionary(FT_Face& face, const char* fontName)
{
	std::map<char, Character> charVector;

	for (unsigned char c = 0; c < 128; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			LOG(LogType::L_ERROR, "Could not load gyph %c", c);
			continue;
		}

		unsigned int texture = 0;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		charVector.insert(std::pair<char, Character>(c, Character(texture, face->glyph->advance.x, face->size->metrics.height, face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap_left, face->glyph->bitmap_top)));
	}

	fontLibrary.emplace(std::pair<std::string, FontDictionary>(fontName, FontDictionary(fontName, charVector)));
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
}


FontDictionary* FreeType_Library::GetFont(const char* name)
{
	std::map<std::string, FontDictionary>::iterator iterator = fontLibrary.find(name);

	if (iterator != fontLibrary.end())
		return &iterator->second;

	else
	{
		ImportNewFont(name, 48);
		iterator = fontLibrary.find(name);

		return (iterator != fontLibrary.end() ? &iterator->second : nullptr);
	}
}