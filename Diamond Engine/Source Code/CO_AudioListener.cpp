#include "CO_AudioListener.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "MO_AudioManager.h"


C_AudioListener::C_AudioListener(GameObject* _gm, bool defaultListener) :Component(_gm), isDefaultListener(false),masterVolume(50)
{
	id = unsigned int(EngineExternal->GetRandomInt());
	EngineExternal->moduleAudio->RegisterNewAudioObject(id);
	SetAsDefaultListener(defaultListener);
}


C_AudioListener::~C_AudioListener()
{
	EngineExternal->moduleAudio->UnRegisterAudioObject(id);
}

#ifndef STANDALONE
bool C_AudioListener::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		bool listenerAux = isDefaultListener;
		if (ImGui::Checkbox("Default Listener", &listenerAux))
		{
			SetAsDefaultListener(listenerAux);
		}

		if (ImGui::SliderFloat("Master Audio Volume", &masterVolume, 0.0f, 100.0f))
		{
			//TODO change master volume here
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE

void C_AudioListener::Update()
{
	//TODO update obj position here
}

void C_AudioListener::SaveData(JSON_Object* nObj)
{
}

void C_AudioListener::LoadData(DEConfig& nObj)
{
}

float C_AudioListener::GetVolume()
{
	return masterVolume;
}

void C_AudioListener::SetVolume(float newVol)
{
	masterVolume = newVol;
}

uint C_AudioListener::GetID()
{
	return id;
}

void C_AudioListener::SetID(uint id)//TODO will it be possible to change Component ID after initialization? if not delete SetID()
{
	EngineExternal->moduleAudio->UnRegisterAudioObject(this->id);
	this->id = id;
	EngineExternal->moduleAudio->RegisterNewAudioObject(id);
}

void C_AudioListener::SetAsDefaultListener(bool setDefault)
{
	if (setDefault)
	{
		if (EngineExternal->moduleAudio->defaultListener != nullptr)
		{
			if (EngineExternal->moduleAudio->defaultListener != this)
			{
				EngineExternal->moduleAudio->defaultListener->SetAsDefaultListener(false);
			}

		}
		EngineExternal->moduleAudio->defaultListener = this;
		EngineExternal->moduleAudio->WwiseListnerHasToUpdate();


	}
	else if (EngineExternal->moduleAudio->defaultListener != nullptr)
	{
		if (EngineExternal->moduleAudio->defaultListener == this)
		{
			EngineExternal->moduleAudio->defaultListener = nullptr;
			EngineExternal->moduleAudio->WwiseListnerHasToUpdate();
		}
	}

	isDefaultListener = setDefault;
}

bool C_AudioListener::IsDefaultListener()
{
	return isDefaultListener;
}
