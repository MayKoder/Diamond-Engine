#pragma once

#include "Module.h"
#include "Window.h"

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

enum class EditorWindow {
	CONFIGURATION,
	CONSOLE,
	MAX
};

class M_Editor : public Module
{

public:
	M_Editor(Application* app, bool start_enabled = true);
	~M_Editor();

	bool Init() override;
	bool Start() override;

	void Draw();
	void DrawMenuBar();

	bool CleanUp() override;

	Window* GetEditorWindow(EditorWindow type);

private:

	std::vector<Window*> windows;


};