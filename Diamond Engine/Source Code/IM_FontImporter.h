#pragma once

#include <map>
#include <vector>
#include <string>

struct FT_FaceRec_;
typedef FT_FaceRec_* FT_Face;
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;

struct Character
{
public:
	Character(unsigned int textureId, unsigned int advanceX, unsigned int advanceY, float sizeX, float sizeY, float bearingX, float bearingY);
	~Character();

public:
	unsigned int textureId;
	unsigned int advance[2];
	int lineDistance;

	float size[2];
	float bearing[2];
};


struct FontDictionary
{
public:
	FontDictionary(const char* name, std::map<char, Character>& characterVec);
	~FontDictionary();

	void UnloadCharacterTextures();

public:
	std::string name;
	std::map<char, Character> characters;
};


class FreeType_Library 
{
public:
	FreeType_Library();
	~FreeType_Library();

	void ImportNewFont(const char* path, int size);
	void InitFontDictionary(FT_Face& face, const char* fontName);
	FontDictionary* GetFont(const char* name);

private:
	FT_Library library;
	
	//Font name	---- Leters of the font
	std::map<std::string, FontDictionary> fontLibrary;
};

