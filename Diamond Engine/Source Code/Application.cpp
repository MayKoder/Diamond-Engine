#include "Application.h"
#include<string>

extern Application* Engine = nullptr;
Application::Application() : quitApplicationState(false), fpsCap(60)
{
	Engine = this;

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	moduleEditor = new M_Editor(this);

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);

	// Renderer last!
	AddModule(renderer3D);
	AddModule(moduleEditor);
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
	LOG("-------------- Application Start --------------");

	// Call Init() in all modules
	for (unsigned int i = 0; i < list_modules.size() && ret == true; i++)
	{
		ret = list_modules[i]->Start();
	}

	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	//Uint32 last_frame_ms = ms_timer.Read();
	//float wait_time = (1000.f / (float)fpsCap) - (float)last_frame_ms;
	//SDL_Delay(static_cast<Uint32>(fabs(wait_time)));
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