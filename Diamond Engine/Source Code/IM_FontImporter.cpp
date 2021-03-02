#include "IM_FontImporter.h"

#include "Globals.h"

#include "FreeType/include/ft2build.h"
#include FT_FREETYPE_H

#pragma comment (lib, "FreeType/win32/freetype.lib")



FreeType_Library::FreeType_Library()
{
	map_of_faces.clear();
	FT_Error error = FT_Init_FreeType(&library);
	if (error)
	{
		LOG(LogType::L_ERROR, "Error initializing FreeType");
	}
}

FreeType_Library::~FreeType_Library()
{
	for (std::map<std::string, FT_Face>::iterator it = map_of_faces.begin(); it != map_of_faces.end(); ++it) {
		FT_Done_Face(it->second);
	}
	FT_Done_FreeType(library);
}

void FreeType_Library::ImportNewFont(const char* path)
{
	FT_Face new_face;
	FT_Error error = FT_New_Face(library, path, 0, &new_face);
	if (error == FT_Err_Unknown_File_Format)
	{
		LOG(LogType::L_ERROR, "The font file could be openedand read, but it appears that its font format is unsupported");
	}
	else if (error)
	{
		LOG(LogType::L_ERROR, "Another error code means that the font file could not be opened or read, or that it is broken...");
	}
	LOG(LogType::L_NORMAL, "The font was loaded correctly");
	map_of_faces.emplace(std::string(path), new_face);
	//GetBitmapTextWithFont(total_fonts, 10, "HEHELOLO");
}

bool FreeType_Library::CheckIfFontExists(const char* path)
{
	
	if (map_of_faces.count(std::string(path)) > 0) {
		return true;
	}
	return false;
	
}


char* FreeType_Library::GetBitmapTextWithFont(const char* index_path, int size_font, const char* text_to_print)
{
	//FT_UInt       glyph_index;
	//int           pen_x, pen_y, n;

	FT_Face face = map_of_faces.find(std::string(index_path))->second;
	/// ARNAU: THIS FUNCTION BELOW WILL BE REVISED WHEN BEING ABLED TO SEE THE TEXT IN SCREEN
	FT_Set_Pixel_Sizes(
		face,   /* handle to face object */
		0,      /* pixel_width           */
		16);   /* pixel_height          */

	/*pen_x = 300;
	pen_y = 200;*/
	FT_Error error;

	char* buffer=nullptr;
	for (int n = 0; n < strlen(text_to_print); n++)
	{
		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Char(face, text_to_print[n], FT_LOAD_RENDER);
		if (error)
			continue;  /* ignore errors */

		  /* now, add the buffer of the bitmat to the buffer that will be returned */
		if (n == 0) {
			buffer = (char*)face->glyph->bitmap.buffer;
		}
		else {
			strcat(buffer, (char*)face->glyph->bitmap.buffer);
		}

		/// ARNAU: LOOK THESE FUNCTIONS WHEN THE TEXT IS PRINTED IN THE SCREEN, BECAUSE PROBABLY IT WILL BE NEEDED FOR PRINTING IT CORRECTLY
		/*my_draw_bitmap(&slot->bitmap,
			pen_x + slot->bitmap_left,
			pen_y - slot->bitmap_top);*/

		/* increment pen position */
		//pen_x += slot->advance.x >> 6;
	}
	return buffer;
}
