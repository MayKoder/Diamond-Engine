#ifndef STANDALONE
#pragma once

#include "Module.h"
#include "Window.h"
#include "Globals.h"
#include "MA_IconSystem.h"
#include "MA_ShortcutManager.h"

#define STYLES_PATH "Settings/styles.json"
#define MAX_STY_INPUT 15

typedef unsigned int ImGuiID;
enum class LogType;
class GameObject;
class ResourceTexture;
struct AssetDir;

enum class EditorWindow {

	//Windows tab
	ASSETS,
	CONSOLE,
	HIERARCHY,
	INSPECTOR,
	SCENE,
	GAME,
	TEXTEDITOR,

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
	virtual ~M_Editor();

	bool Init() override;
	bool Start() override;

	void Draw();
	void DrawMenuBar();
	void DrawTopBar();
	void CreateDockSpace();

	bool CleanUp() override;

	void DrawCreateMenu();
	Window* GetEditorWindow(EditorWindow type);

	void SaveStyle(const char* styleName);
	void ChangeStyleTo(const char* styleName);
	void DeleteStyle(const char* styleName);
	void UpdateLoadedStylesVector(std::vector<std::string>* _styles);

	GameObject* GetSelectedGO();
	GameObject* GetDraggingGO();
	void SetSelectedGO(GameObject* _obj);

	AssetDir* GetSelectedAsset();
	void SetSelectedAsset(AssetDir* _file);

	void ClearConsole();
	void LogToConsole(const char* msg, LogType _type = LogType::L_NORMAL);

	IconManager editorIcons;
	ShortcutManager shortcutManager;

private:
	std::vector<Window*> windows;
	std::vector<std::string> styles;

	//WARNING TODO: This is bad find a better way
	char styleInput[MAX_STY_INPUT] = "";

	bool displayWindow;

	float viewportCorSize;
	ImGuiID dockspace_id;

	ImVec4 playingTint;
};

#endif