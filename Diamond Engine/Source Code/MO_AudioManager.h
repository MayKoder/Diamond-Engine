#pragma once
#ifndef __MODULEAUDIOMANAGER_H__
#define __MODULEAUDIOMANAGER_H__

#include <string>
#include <vector>

#include "Wwise/AK/SoundEngine/Common/AkTypes.h"

#include "Module.h"
#include "Globals.h"

class ModuleAudioManager :public Module
{
public:

	// Construct / Destruct Methods
	ModuleAudioManager(Application* app, bool start_enabled = true);
	~ModuleAudioManager();

	// Application
	bool Init();
	bool Start();
	
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();
	//

	// Audio Management
	void RegisterNewAudioObject(unsigned int id);
	void UnRegisterAudioObject(unsigned int id);

	void StopAllSounds() const;
	void PauseAllSounds() const;
	void ResumeAllSounds() const;

	bool LoadBank(std::string& name);

public:

};


#endif // !__MODULEAUDIOMANAGER_H__