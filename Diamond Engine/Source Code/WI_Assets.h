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
	void DrawCreationWindow();
	void DrawCreationPopup(const char* popName, const char* dotExtension, std::function<void(const char*)> f);

	void DrawPathButtons();
	void DrawCurrentFolder();
	void SetFilePayload(AssetDir& file);

	AssetDir* selectedFile;
	AssetDir* bigDisplayFolder;
	AssetDir* displayFolder;

private:
	float tree_window_width;
};



#endif //__W_ASSETS_H__

#endif // !STANDALONE