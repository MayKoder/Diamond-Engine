#pragma once

#include "Timer.h"
#include <vector>
#include<string>

#include "Module.h"
class M_FileSystem;
class ModuleWindow;
class ModuleInput;
class ModuleRenderer3D;
class ModuleCamera3D;
class M_Scene;
class M_Editor;
class M_ResourceManager;
class M_MonoManager;

#include"MathGeoLib/include/Algorithm/Random/LCG.h"

class Application
{
public:
	M_FileSystem* moduleFileSystem = nullptr;
	ModuleWindow* moduleWindow = nullptr;
	ModuleInput* moduleInput = nullptr;
	ModuleRenderer3D* moduleRenderer3D = nullptr;
	ModuleCamera3D* moduleCamera = nullptr;
	M_Scene* moduleScene = nullptr;
	M_Editor* moduleEditor = nullptr;
	M_ResourceManager* moduleResources = nullptr;
	M_MonoManager* moduleMono = nullptr;

	std::vector<Module*> list_modules;

private:

	Timer	ms_timer;
	float	dt;
	bool quitApplicationState;

	//TODO: Maybe move inside a module?
	LCG randomizer;

public:
	int fpsCap;

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void ExitApplication();
	bool GetExitState();

	float GetDT() const;
	float GetFrameRate() const;
	
	int GetRandomInt();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* EngineExternal;