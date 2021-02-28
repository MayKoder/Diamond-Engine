#pragma once
#ifndef __MODULEAUDIOMANAGER_H__
#define __MODULEAUDIOMANAGER_H__

#include <string>
#include <vector>

#include "Wwise/AK/SoundEngine/Common/AkTypes.h"
#include "MathGeoLib/include/MathGeoLibFwd.h"

#include "Module.h"
#include "Globals.h"
#include "AudioBank.h"

class C_AudioListener;
class C_AudioSource;

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

	void PlayEvent(unsigned int id, std::string& eventName);
	void StopEvent(unsigned int id) const;
	void PauseEvent(unsigned int id) const;
	void ResumeEvent(unsigned int id) const;

	void ChangeRTPCValue(unsigned int id, std::string& RTPCname, float value);

	bool LoadBank(std::string& name);

	void WwiseListnerHasToUpdate();

	void AddAudioSource(C_AudioSource* new_source);
	void RemoveAudioSource(C_AudioSource* source);

	void SetAudioObjTransform(unsigned int id,float4x4& transform);

private:
	void UpdateWwiseListener();

public:
	std::vector<AudioBank> banks;
	std::vector<C_AudioSource*> audio_sources;
	C_AudioListener* defaultListener;

private:
	bool wwiseListenerHasToUpdate;
};


#endif // !__MODULEAUDIOMANAGER_H__