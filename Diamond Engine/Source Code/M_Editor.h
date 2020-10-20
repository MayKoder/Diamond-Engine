#pragma once

#include "Module.h"
#include "Window.h"

#define STYLES_PATH "styles.json"

typedef unsigned int ImGuiID;
enum class EditorWindow {

	//Windows tab
	ASSETS,
	CONSOLE,
	HIERARCHY,
	INSPECTOR,
	SCENE,

	//Help tab
	ABOUT,

	//Edit tab
	CONFIGURATION,
	
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
	void DeleteStyle(const char* styleName);

	void LogToConsole(const char* msg);

private:

	std::vector<Window*> windows;
	bool displayWindow;

	float viewportCorSize;
	ImGuiID dockspace_id;
};