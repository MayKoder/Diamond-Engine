#include "Application.h"
#include<string>
#include "Globals.h"

#include "MO_FileSystem.h"
#include "MO_Window.h"
#include "MO_Input.h"
#include "MO_Renderer3D.h"
#include "MO_Camera3D.h"
#include "MO_Scene.h"
#include "MO_Physics.h"
#include "MO_AudioManager.h"
#include "MO_GUI.h"

#ifndef STANDALONE
#include "MO_Editor.h"
#endif // !STANDALONE


#include "MO_ResourceManager.h"
#include "MO_MonoManager.h"

#include"DETime.h"


//TODO: Change all std::string in methods to std::string&
extern Application* EngineExternal = nullptr;
Application::Application() : quitApplicationState(false), fpsCap(60)
{
	EngineExternal = this;

	moduleFileSystem = new M_FileSystem(this);
	moduleWindow = new ModuleWindow(this);
	moduleInput = new ModuleInput(this);
	moduleScene = new M_Scene(this);
	moduleGui = new M_Gui(this);
	moduleRenderer3D = new ModuleRenderer3D(this);
	moduleCamera = new ModuleCamera3D(this);
	modulePhysics = new ModulePhysics(this);
	moduleAudio = new ModuleAudioManager(this);
	moduleMono = new M_MonoManager(this);

	moduleResources = new M_ResourceManager(this);

#ifndef STANDALONE
	moduleEditor = new M_Editor(this);
#endif

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(moduleFileSystem);
	AddModule(moduleWindow);
	AddModule(moduleInput);

	//Should go before the scene
	AddModule(moduleAudio);

	//Should scene be here?
	AddModule(modulePhysics);
	AddModule(moduleScene);
	AddModule(moduleGui);

	// Renderer last!
	AddModule(moduleCamera);

	AddModule(moduleMono);

	AddModule(moduleResources);

#ifndef STANDALONE
	AddModule(moduleEditor);
#endif

	AddModule(moduleRenderer3D);
}


Application::~Application()
{
	for (int i = list_modules.size() - 1; i >= 0; --i)
	{
		delete list_modules[i];
		list_modules[i] = nullptr;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (unsigned int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG(LogType::L_NORMAL, "-------------- Application Start --------------");

	// Call Init() in all modules
	for (unsigned int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Start();
	}

	ms_timer.Start();
	DETime::realStartTime = ms_timer.GetStartTime();

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;

	//TODO, ASK: Should i do this here?
	DETime::realTimeDeltaTime = dt;
	DETime::realTimeSinceStartup += DETime::realTimeDeltaTime;
	DETime::PreUpdate();

	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	if (moduleRenderer3D->vsync) 
	{
		Uint32 last_frame_ms = ms_timer.Read();
		float wait_time = (1000.f / (float)fpsCap) - (float)last_frame_ms;
		SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
	}
}

void Application::ExitApplication()
{

	//If there is unsaved changes, ask if they want to save
	//We can't quit ASAP, we need to clean everything up first
	quitApplicationState = true;


}

//Returs true if the engine is waiting to quit 
bool Application::GetExitState()
{
	return quitApplicationState;
}

float Application::GetDT() const
{
	return dt;
}

float Application::GetFrameRate() const
{
	return 1.f / GetDT();
}

int Application::GetRandomInt() //TODO: We could add iconCount here to make sure we don't get duplicated id's?
{
	return randomizer.Int();
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	for (unsigned int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PreUpdate(dt);
	}
	for (unsigned int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->Update(dt);
	}
	for (unsigned int i = 0; i < list_modules.size() && ret == UPDATE_CONTINUE; i++)
	{
		ret = list_modules[i]->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = list_modules.size() - 1; i >= 0 && ret == true; --i)
	{
		ret = list_modules[i]->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}