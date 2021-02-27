#include "CO_AudioSource.h"
#include "MO_AudioManager.h"
#include "Application.h"

C_AudioSource::C_AudioSource(GameObject* _gm): Component(_gm)
{
}

C_AudioSource::~C_AudioSource()
{
	EngineExternal->moduleAudio->RemoveAudioSource(this);
}

#ifndef STANDALONE
bool C_AudioSource::OnEditor()
{
	return false;
}
#endif // !STANDALONE

void C_AudioSource::Update()
{
}

void C_AudioSource::SaveData(JSON_Object* nObj)
{
}

void C_AudioSource::LoadData(DEConfig& nObj)
{
}

std::string& C_AudioSource::GetEventName(AudioBank* reference)
{
	reference = audBankReference;
	return evName;
}

void C_AudioSource::SetEventName(std::string& newEventName)
{
	this->evName = newEventName;
}

void C_AudioSource::SetBankReference(AudioBank* ref)
{
	this->audBankReference = ref;
}

float C_AudioSource::GetVolume()
{
	return this->volume;
}

void C_AudioSource::SetVolume(float newVol)
{
	this->volume = newVol;
	// TODO: change audio rtpc
}

float C_AudioSource::GetPitch()
{
	return this->pitch;
}

void C_AudioSource::SetPitch(float newPitch)
{
	this->pitch = newPitch;
	// TODO: change audio rtpc
}

void C_AudioSource::PlayEvent()
{
	if (this->IsActive())
		EngineExternal->moduleAudio->PlayEvent(this->id, this->evName);
}

void C_AudioSource::PauseEvent()
{
	EngineExternal->moduleAudio->PauseEvent(this->id);
}

void C_AudioSource::ResumeEvent()
{
	EngineExternal->moduleAudio->ResumeEvent(this->id);
}

void C_AudioSource::StopEvent()
{
	EngineExternal->moduleAudio->StopEvent(this->id);
}

bool C_AudioSource::IsMuted()
{
	return this->isMuted;
}

void C_AudioSource::SetMuted(bool muted)
{
	// TODO: change audio rtpc (set volume to 0 or saved volume level)
}
