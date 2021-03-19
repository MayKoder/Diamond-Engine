#pragma once
#ifndef __CO_AUDIOSOURCE_H__
#define __CO_AUDIOSOURCE_H__

#include "Component.h"
#include "AudioBank.h"

class C_Transform;

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
	void SetBankReference(AudioBank* ref);

	float GetVolume();
	void SetVolume(float newVol);

	float GetPitch();
	void SetPitch(float newPitch);

	bool GetPlayOnAwake() const;

	void PlayEvent();
	void PauseEvent();
	void ResumeEvent();
	void StopEvent();

	unsigned int GetWwiseID();

	bool IsMuted();
	void SetMuted(bool muted);

private:

	std::string		evName;
	std::string		audBankName;
	AudioBank*		audBankReference;
	float			volume;
	float			pitch;
	bool			playOnAwake;
	bool			isMuted;
	unsigned int	id;

	C_Transform*	gameObjectTransform;
};

#endif // !__CO_AUDIOSOURCE_H__