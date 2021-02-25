#pragma once
#include "FreeType/include/freetype/freetype.h"

namespace FontImporter{

	
	
	
}

class FreeType_Library {
public:
	FreeType_Library();
	virtual ~FreeType_Library();

	bool ImportNewFont(const char* path);

	int GetTotalFonts();

private:
	FT_Library library;
	FT_Face faces[10];
	int total_fonts;

};

