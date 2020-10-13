#pragma once

#include "Module.h"
#include "Window.h"

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

//Window types
#include "W_Configuration.h"
#include "W_Console.h"
#include "W_About.h"
#include "W_Inspector.h"
#include "W_Hierarchy.h"
#include "W_Scene.h"

#define STYLES_PATH "styles.json"

enum class EditorWindow {
	CONFIGURATION,
	CONSOLE,
	ABOUT,
	INSPECTOR,
	HIERARCHY,
	SCENE,
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
	void DrawTopBar();
	void CreateDockSpace();

	bool CleanUp() override;

	Window* GetEditorWindow(EditorWindow type);

	void SaveStyle(const char* styleName);
	void LoadStyle(const char* styleName);

private:

	std::vector<Window*> windows;
	bool displayWindow;

	float viewportCorSize;
	ImGuiID dockspace_id;


};