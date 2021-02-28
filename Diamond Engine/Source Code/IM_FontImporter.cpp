#include "IM_FontImporter.h"

#include "Globals.h"

#include "FreeType/include/ft2build.h"
#include FT_FREETYPE_H

#pragma comment (lib, "FreeType/win32/freetype.lib")



FreeType_Library::FreeType_Library() : total_fonts(0)
{
	memset(faces, 0, sizeof(FT_Face) * TOTAL_NUM_OF_FONTS);
	for (int i = 0; i < TOTAL_NUM_OF_FONTS; i++) {
		path_to_index[i] = "";
	}
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

int FreeType_Library::ImportNewFont(const char* path)
{
	if (total_fonts == TOTAL_NUM_OF_FONTS) {
		LOG(LogType::L_WARNING, "There is not enough space for more fonts, to increase the limit, change the define TOTAL_NUM_OF_FONTS");
		return -1;
	}
	FT_Error error = FT_New_Face(library, path, 0, &faces[total_fonts]);
	if (error == FT_Err_Unknown_File_Format)
	{
		LOG(LogType::L_ERROR, "The font file could be openedand read, but it appears that its font format is unsupported");
		return -1;
	}
	else if (error)
	{
		LOG(LogType::L_ERROR, "Another error code means that the font file could not be opened or read, or that it is broken...");
		return -1;
	}
	LOG(LogType::L_NORMAL, "The font was loaded correctly");
	path_to_index[total_fonts] = path;
	GetBitmapTextWithFont(total_fonts, 10, "HEHELOLO");
	return total_fonts++;
}

int FreeType_Library::GetFont(const char* path)
{
	for (int i = 0; i < total_fonts; i++) {
		if (strcmp(path, path_to_index[i]) == 0) {
			return i;
		}
	}
	return -1;
}

int FreeType_Library::GetTotalFonts()
{
	return total_fonts;
}

char* FreeType_Library::GetBitmapTextWithFont(int index_font, int size_font, const char* text_to_print)
{
	//FT_UInt       glyph_index;
	//int           pen_x, pen_y, n;


	/// ARNAU: THIS FUNCTION BELOW WILL BE REVISED WHEN BEING ABLED TO SEE THE TEXT IN SCREEN
	FT_Set_Pixel_Sizes(
		faces[index_font],   /* handle to face object */
		0,      /* pixel_width           */
		16);   /* pixel_height          */

	/*pen_x = 300;
	pen_y = 200;*/
	FT_Error error;

	char* buffer=nullptr;
	for (int n = 0; n < strlen(text_to_print); n++)
	{
		/* load glyph image into the slot (erase previous one) */
		error = FT_Load_Char(faces[index_font], text_to_print[n], FT_LOAD_RENDER);
		if (error)
			continue;  /* ignore errors */

		  /* now, add the buffer of the bitmat to the buffer that will be returned */
		if (n == 0) {
			buffer = (char*)faces[index_font]->glyph->bitmap.buffer;
		}
		else {
			strcat(buffer, (char*)faces[index_font]->glyph->bitmap.buffer);
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
