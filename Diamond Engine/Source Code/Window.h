#ifndef STANDALONE

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <string>
#include "ImGui/imgui.h"

class Window
{

public:
	Window();
	virtual ~Window();

	virtual void Draw() = 0;

	std::string name;
	bool active;

};

#endif //__WINDOW_H__

#endif // !STANDALONE
