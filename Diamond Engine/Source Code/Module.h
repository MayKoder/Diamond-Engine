#pragma once

class Application;

//Moved this here to avoid problems with Globals.h
enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

class Module
{
private :
	bool enabled;

public:
	Application* App;

	Module(Application* parent, bool start_enabled = true) : App(parent), enabled(start_enabled)
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

#ifndef STANDALONE
	virtual void OnGUI() 
	{

	}
#endif // !STANDALONE

};