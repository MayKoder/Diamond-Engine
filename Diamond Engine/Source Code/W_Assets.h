#ifndef __W_ASSETS_H__
#define __W_ASSETS_H__

#include "Window.h"
#include<vector>

//TODO: Move AssetFile to a empty file to avoid adding all filesystem here
#include"M_FileSystem.h"

class W_Assets : public Window
{

public:
	W_Assets();
	virtual ~W_Assets();

	void PopulateFileArray();

	void Draw() override;

	void DrawFileTree(AssetDir& file);

	AssetDir rootFile;


};



#endif //__W_ASSETS_H__
