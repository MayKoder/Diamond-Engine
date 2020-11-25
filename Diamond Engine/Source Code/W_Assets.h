#ifndef __W_ASSETS_H__
#define __W_ASSETS_H__

#include "Window.h"
#include<vector>

class W_Assets : public Window
{

public:
	W_Assets();
	virtual ~W_Assets();

	void PopulateFileArray();

	void Draw() override;

	std::vector<std::string> files;

};



#endif //__W_ASSETS_H__
