#pragma once

#include "Module.h"
#include "Window.h"

#define STYLES_PATH "Settings/styles.json"
#define MAX_STY_INPUT 15

typedef unsigned int ImGuiID;
enum class LogType;
class GameObject;
class ResourceTexture;
class AssetDir;

enum class EditorWindow {

	//Windows tab
	ASSETS,
	CONSOLE,
	HIERARCHY,
	INSPECTOR,
	SCENE,
	GAME,

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

	Window* GetEditorWindow(EditorWindow type);

	void SaveStyle(const char* styleName);
	void ChangeStyleTo(const char* styleName);
	void DeleteStyle(const char* styleName);
	void UpdateLoadedStylesVector(std::vector<std::string>* _styles);

	GameObject* GetSelectedGO();
	void SetSelectedGO(GameObject* _obj);

	AssetDir* GetSelectedAsset();
	void SetSelectedAsset(AssetDir* _file);

	void LogToConsole(const char* msg, LogType _type = LogType::L_NORMAL);

private:


	std::vector<Window*> windows;
	std::vector<std::string> styles;

	std::vector<ResourceTexture*> editorIcons;

	//WARNING TODO: This is bad find a better way
	char styleInput[MAX_STY_INPUT] = "";
	
	//TODO: Save current style and load it at init to keep styles between uses
	//std::string currentStyle;

	bool displayWindow;

	float viewportCorSize;
	ImGuiID dockspace_id;
};