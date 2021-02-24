#pragma once
#include "FreeType/include/freetype/freetype.h"

namespace FontImporter{

	
	
	
}

class FreeType_Library {
public:
	FreeType_Library();
	virtual ~FreeType_Library();

private:
	FT_Library library;

};

