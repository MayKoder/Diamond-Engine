#ifndef STANDALONE

#ifndef __W_ABOUT_H__
#define __W_ABOUT_H__

#include "Window.h"

class W_About : public Window
{

public:
	W_About();
	virtual ~W_About();

	void Draw() override;

	std::string physVersion;
	std::string assimpVersion;

};

#endif //__W_ABOUT_H__

#endif // !STANDALONE