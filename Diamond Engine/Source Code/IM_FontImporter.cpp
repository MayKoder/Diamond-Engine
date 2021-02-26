#include "IM_FontImporter.h"

#include "Globals.h"

#include "FreeType/include/ft2build.h"
#include FT_FREETYPE_H

#pragma comment (lib, "FreeType/win32/freetype.lib")



FreeType_Library::FreeType_Library() : total_fonts(0)
{
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		LOG(LogType::L_ERROR, "Error initializing FreeType");
	}
}

FreeType_Library::~FreeType_Library()
{
	for (int i = total_fonts-1; i >= 0; i--) {
		FT_Done_Face(faces[i]);
	}
	FT_Done_FreeType(library);
}

bool FreeType_Library::ImportNewFont(const char* path)
{
	if (total_fonts == 10) {
		LOG(LogType::L_WARNING, "There is not enough space for more fonts");
		return false;
	}
	FT_Error error = FT_New_Face(library, path, 0, &faces[total_fonts]);
	if (error == FT_Err_Unknown_File_Format)
	{
		LOG(LogType::L_ERROR, "The font file could be openedand read, but it appears that its font format is unsupported");
		return false;
	}
	else if (error)
	{
		LOG(LogType::L_ERROR, "Another error code means that the font file could not be opened or read, or that it is broken...");
		return false;
	}
	LOG(LogType::L_NORMAL, "The font was loaded correctly");
	++total_fonts;
	return true;
}

int FreeType_Library::GetTotalFonts()
{
	return total_fonts;
}
