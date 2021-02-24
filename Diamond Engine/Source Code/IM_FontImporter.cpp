#include "IM_FontImporter.h"

#include "Globals.h"

#include "FreeType/include/ft2build.h"
#include FT_FREETYPE_H

#pragma comment (lib, "FreeType/win32/freetype.lib")



FreeType_Library::FreeType_Library()
{
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		LOG(LogType::L_ERROR, "Error initializing FreeType");
	}
}

FreeType_Library::~FreeType_Library()
{
	FT_Done_FreeType(library);
}
