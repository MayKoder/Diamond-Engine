#pragma once
#ifndef __CO_AUDIOSOURCE_H__
#define __CO_AUDIOSOURCE_H__

#include "Component.h"
#include "AudioBank.h"

class C_AudioSource : public Component
{
public:
	C_AudioSource(GameObject* _gm);
	virtual ~C_AudioSource();

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void Update() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	std::string& GetEventName(AudioBank* reference = nullptr);
	void SetEventName(std::string& newEventName);

	float GetVolume();
	void SetVolume(float newVol);

	float GetPitch();
	void SetPitch(float newPitch);

	void PlayEvent();
	void PauseEvent();
	void ResumeEvent();
	void StopEvent();

	bool IsMuted();
	void SetMuted(bool muted);

private:

	std::string		evName;
	AudioBank*		audBankReference;
	float			volume;
	float			pitch;
	bool			playOnAwake;
	bool			isMuted;
	uint			id;
};

#endif // !__CO_AUDIOSOURCE_H__