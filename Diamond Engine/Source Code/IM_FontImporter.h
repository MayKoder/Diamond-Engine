#pragma once
#include "FreeType/include/freetype/freetype.h"
#include <map>
#include <string>


namespace FontImporter{

	
	
	
}

class FreeType_Library {
public:
	FreeType_Library();
	~FreeType_Library();

	void ImportNewFont(const char* path);

	bool CheckIfFontExists(const char* path);


	char* GetBitmapTextWithFont(const char* index_path, int size_font, const char* text_to_print);

private:
	FT_Library library;
	
	std::map<std::string, FT_Face> map_of_faces;

};

