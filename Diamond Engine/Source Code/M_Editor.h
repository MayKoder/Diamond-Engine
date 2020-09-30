#pragma once

#include "Module.h"
#include "Window.h"

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "W_Configuration.h"

class M_Editor : public Module
{

public:
	M_Editor(Application* app, bool start_enabled = true);
	~M_Editor();

	bool Init();
	bool Start();

	void Draw();
	void DrawMenuBar();

	bool CleanUp();

private:

	std::vector<Window*> windows;


};