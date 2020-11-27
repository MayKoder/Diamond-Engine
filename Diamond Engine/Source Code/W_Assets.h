#ifndef __W_ASSETS_H__
#define __W_ASSETS_H__

#include "Window.h"
#include<vector>

class AssetDir;

class W_Assets : public Window
{

public:
	W_Assets();
	virtual ~W_Assets();

	void Draw() override;

	void DrawFileTree(AssetDir& file);

	AssetDir* selectedFile;
};



#endif //__W_ASSETS_H__
