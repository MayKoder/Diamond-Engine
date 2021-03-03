#pragma once

#include "Globals.h"
#include "Application.h"
#include "GameObject.h"
#include "CO_AudioSource.h"
#include "Component.h"

void PlayAudio(MonoObject* go, MonoString* eventAudio)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);
	std::string ev = mono_string_to_utf8(eventAudio);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		audSource->SetEventName(ev);
		audSource->PlayEvent();
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't play the audio %s. Component was null pointer", ev.c_str());
	}
}

void ResumeAudio(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		audSource->ResumeEvent();
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't resume the audio. Component was null pointer");
	}
}

void PauseAudio(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		audSource->PauseEvent();
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't pause the audio. Component was null pointer");
	}
}

void StopAudio(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		audSource->StopEvent();
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't stop the audio. Component was null pointer");
	}
}

float GetVolume(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return NULL;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		return audSource->GetVolume();
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't get the volume. Component was null pointer");
		return NULL;
	}
}

void SetVolume(MonoObject* go, float volume) 
{
	if (EngineExternal == nullptr)
		return;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		audSource->SetVolume(volume);
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't set the volume to %.2f. Component was null pointer", volume);
	}
}

float GetPitch(MonoObject* go) 
{
	if (EngineExternal == nullptr)
		return NULL;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		return audSource->GetPitch();
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't get the pitch. Component was null pointer");
		return NULL;
	}
}

void SetPitch(MonoObject* go, float pitch) 
{
	if (EngineExternal == nullptr)
		return;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		audSource->SetPitch(pitch);
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't set the pitch to %.2f. Component was null pointer", pitch);
	}
}

bool GetMuted(MonoObject* go)
{
	if (EngineExternal == nullptr)
		return NULL;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		return audSource->IsMuted();
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't get the mute variable. Component was null pointer");
	}
}

void SetMuted(MonoObject* go, bool muted) 
{
	if (EngineExternal == nullptr)
		return;

	GameObject* GO = EngineExternal->moduleMono->GameObject_From_CSGO(go);

	C_AudioSource* audSource = dynamic_cast<C_AudioSource*>(GO->GetComponent(Component::TYPE::AUDIO_SOURCE));
	if (audSource != nullptr)
	{
		audSource->SetMuted(muted);
	}
	else
	{
		LOG(LogType::L_WARNING, "Couldn't set the mute to the given value. Component was null pointer");
	}
}