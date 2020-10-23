#pragma once

#include "Module.h"
#include "Window.h"

#define STYLES_PATH "styles.json"
#define MAX_STY_INPUT 15

typedef unsigned int ImGuiID;
enum class LogType;

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
	void ChangeStyleTo(const char* styleName);
	void DeleteStyle(const char* styleName);
	void UpdateLoadedStylesVector(std::vector<std::string>* _styles);

	void LogToConsole(const char* msg, LogType _type = LogType::L_NORMAL);

private:

	//ASK:
	//Could use array pointers?
	//Would it be faster?
	std::vector<Window*> windows;
	std::vector<std::string> styles;

	//WARNING TODO: This is bad fins a better way
	char styleInput[MAX_STY_INPUT] = "";

	bool displayWindow;

	float viewportCorSize;
	ImGuiID dockspace_id;
};