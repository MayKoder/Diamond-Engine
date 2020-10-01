#pragma once
#ifndef __W_CONFIGURATION_H__
#define __W_CONFIGURATION_H__

#include "Window.h"
#include <vector>
#include "Application.h"
#include "MaykMath.h"

#define FPS_MS_LOG_MAXLENGHT 50

class W_Configuration : public Window
{

public:
	W_Configuration();
	virtual ~W_Configuration();

	void Draw() override;

	void UpdateInfoLogs();

	bool displayWindow;

private:
	std::vector<float> fps_log;
	std::vector<float> ms_log;

};



#endif //__W_CONFIGURATION_H__
