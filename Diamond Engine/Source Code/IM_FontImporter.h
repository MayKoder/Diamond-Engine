#pragma once
#include "FreeType/include/freetype/freetype.h"

#define TOTAL_NUM_OF_FONTS 10

namespace FontImporter{

	
	
	
}

class FreeType_Library {
public:
	FreeType_Library();
	~FreeType_Library();

	int ImportNewFont(const char* path);

	int GetFont(const char* path);

	int GetTotalFonts();

	char* GetBitmapTextWithFont(int index_font, int size_font, const char* text_to_print);

private:
	FT_Library library;
	
	FT_Face faces[TOTAL_NUM_OF_FONTS];
	const char* path_to_index[TOTAL_NUM_OF_FONTS];
	int total_fonts;

};

