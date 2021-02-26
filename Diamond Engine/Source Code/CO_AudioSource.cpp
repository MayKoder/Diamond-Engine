#include "CO_AudioSource.h"
#include "MO_AudioManager.h"

C_AudioSource::C_AudioSource(GameObject* _gm): Component(_gm)
{
}

C_AudioSource::~C_AudioSource()
{
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
}

float C_AudioSource::GetVolume()
{
	return 0.0f;
}

void C_AudioSource::SetVolume(float newVol)
{
}

float C_AudioSource::GetPitch()
{
	return 0.0f;
}

void C_AudioSource::SetPitch(float newPitch)
{
}

void C_AudioSource::PlayEvent()
{
}

void C_AudioSource::PauseEvent()
{
}

void C_AudioSource::ResumeEvent()
{
}

void C_AudioSource::StopEvent()
{
}

bool C_AudioSource::IsMuted()
{
	return false;
}

void C_AudioSource::SetMuted(bool muted)
{
}
