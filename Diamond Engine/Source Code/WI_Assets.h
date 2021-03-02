#ifndef STANDALONE

#ifndef __W_ASSETS_H__
#define __W_ASSETS_H__

#include "Window.h"
#include<vector>
#include<functional>

struct AssetDir;

class W_Assets : public Window
{

public:
	W_Assets();
	virtual ~W_Assets();

	void Draw() override;

	void DrawFileTree(AssetDir& file);
	void DrawCreationPopup(const char* popName, const char* dotExtension, std::function<void(const char*)> f);

	AssetDir* selectedFile;
	AssetDir* bigDisplayFolder;
	AssetDir* displayFolder;
};



#endif //__W_ASSETS_H__

#endif // !STANDALONE