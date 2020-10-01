#pragma once
#ifndef __W_CONFIGURATION_H__
#define __W_CONFIGURATION_H__

#include "Window.h"
#include <vector>
#include "Application.h"

class W_Configuration : public Window
{

public:
	W_Configuration();
	virtual ~W_Configuration();

	void Draw() override;

	bool displayWindow;

};



#endif //__W_CONFIGURATION_H__

