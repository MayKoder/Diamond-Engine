#include "CO_AudioSource.h"
#include "Application.h"
#include "MO_AudioManager.h"
#include "GameObject.h"
#include "CO_Transform.h"

#include "ImGui/imgui.h"

C_AudioSource::C_AudioSource(GameObject* _gm) : Component(_gm), audBankReference(nullptr), evName(""), isMuted(false), pitch(50.0f), playOnAwake(false), volume(50.0f), audBankName("")
{
	name = "Audio Source";
	id = static_cast<unsigned int>(EngineExternal->GetRandomInt());
	gameObjectTransform = dynamic_cast<C_Transform*>(gameObject->GetComponent(Component::TYPE::TRANSFORM));
	EngineExternal->moduleAudio->RegisterNewAudioObject(id);
	EngineExternal->moduleAudio->AddAudioSource(this);
}

C_AudioSource::~C_AudioSource()
{
	EngineExternal->moduleAudio->StopComponent(id);
	EngineExternal->moduleAudio->RemoveAudioSource(this);
	EngineExternal->moduleAudio->UnRegisterAudioObject(id);
	gameObjectTransform = nullptr;
	audBankReference = nullptr;

}

#ifndef STANDALONE
bool C_AudioSource::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (ImGui::BeginCombo("Audio Bank", audBankName.c_str()))
		{
			std::vector<AudioBank*>::const_iterator it;
			for (it = EngineExternal->moduleAudio->banks.begin(); it != EngineExternal->moduleAudio->banks.end(); ++it)
			{
				bool isSelected = (audBankName == (*it)->bank_name);
				if (ImGui::Selectable((*it)->bank_name.c_str()))
				{
					audBankReference = (*it);
					audBankName = (*it)->bank_name;
					if (!(*it)->loaded_in_heap)
					{
						(*it)->loaded_in_heap = EngineExternal->moduleAudio->LoadBank(audBankReference->bank_name);
					}
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (audBankReference != nullptr && ImGui::BeginCombo("Audio to Play", evName.c_str()))
		{
			std::map<uint64, std::string>::const_iterator ev_it;
			for (ev_it = audBankReference->events.begin(); ev_it != audBankReference->events.end(); ++ev_it)
			{
				bool isSelected = (evName == (*ev_it).second);
				if (ImGui::Selectable((*ev_it).second.c_str()))
				{
					evName = (*ev_it).second;
				}
				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		if (ImGui::Button("Play"))
		{
			PlayEvent();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{
			StopEvent();
		}

		ImGui::Checkbox("Play on Awake", &playOnAwake);

		if (ImGui::SliderFloat("Volume", &volume, 0.0f, 99.99f))
		{
			SetVolume(volume);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Mute", &isMuted))
		{
			SetMuted(isMuted);
		}

		if (ImGui::SliderFloat("Pitch", &pitch, 0.0f, 100.0f))
		{
			SetPitch(pitch);
		}

		return true;
	}
	return false;
}
#endif // !STANDALONE

void C_AudioSource::Update()
{
	float3 pos = gameObjectTransform->globalTransform.TranslatePart();
	EngineExternal->moduleAudio->SetAudioObjTransform(this->id, pos, gameObjectTransform->GetForward(), gameObjectTransform->GetUp());
}

void C_AudioSource::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteString(nObj, "evName", this->evName.c_str());

	if (this->audBankReference != nullptr)
		DEJson::WriteString(nObj, "audBankReference", this->audBankReference->bank_name.c_str());

	DEJson::WriteFloat(nObj, "volume", this->volume);
	DEJson::WriteFloat(nObj, "pitch", this->pitch);
	DEJson::WriteBool(nObj, "playOnAwake", this->playOnAwake);
	DEJson::WriteBool(nObj, "isMuted", this->isMuted);
}

void C_AudioSource::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	this->evName = nObj.ReadString("evName");
	std::string bankName = nObj.ReadString("audBankReference");
	SetMuted(nObj.ReadBool("isMuted"));
	SetVolume(nObj.ReadFloat("volume"));
	SetPitch(nObj.ReadFloat("pitch"));
	this->playOnAwake = nObj.ReadBool("playOnAwake");

	// Iterate and assign audio bank. If not loaded, load
	std::vector<AudioBank*>::iterator it;
	for (it = EngineExternal->moduleAudio->banks.begin(); it != EngineExternal->moduleAudio->banks.end(); ++it)
	{
		if ((*it)->bank_name == bankName)
		{
			audBankReference = (*it);
			audBankName = bankName;

			if (!(*it)->loaded_in_heap)
			{
				EngineExternal->moduleAudio->LoadBank(bankName);
				(*it)->loaded_in_heap = true;
			}
			return;
		}
	}
	audBankReference = nullptr;
	LOG(LogType::L_WARNING, "Audio Bank called %s, has not been found during scene loading...", bankName);
}

std::string& C_AudioSource::GetEventName(AudioBank* reference)
{
	reference = nullptr;
	if (audBankReference != nullptr)
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
	this->volume = MIN(newVol, 99.99f);
	this->volume = MAX(newVol, 0.0f);
	if (!isMuted && evName != "")
		EngineExternal->moduleAudio->ChangeRTPCValue(this->id, std::string("SourceVolume"), this->volume);
}

float C_AudioSource::GetPitch()
{
	return this->pitch;
}

void C_AudioSource::SetPitch(float newPitch)
{
	this->pitch = MIN(newPitch, 100);
	this->pitch = MAX(newPitch, 0);
	if (!isMuted)
		EngineExternal->moduleAudio->ChangeRTPCValue(this->id, std::string("SourcePitch"), this->pitch);
}

bool C_AudioSource::GetPlayOnAwake() const
{
	return this->playOnAwake;
}

void C_AudioSource::PlayEvent()
{
	if (this->IsActive())
		EngineExternal->moduleAudio->PlayEvent(this->id, this->evName);
}

void C_AudioSource::PauseEvent()
{
	EngineExternal->moduleAudio->PauseEvent(this->id, this->evName);
}

void C_AudioSource::ResumeEvent()
{
	EngineExternal->moduleAudio->ResumeEvent(this->id, this->evName);
}

void C_AudioSource::StopEvent()
{
	EngineExternal->moduleAudio->StopEvent(this->id, this->evName);
}

unsigned int C_AudioSource::GetWwiseID()
{
	return id;
}

bool C_AudioSource::IsMuted()
{
	return this->isMuted;
}

void C_AudioSource::SetMuted(bool muted)
{
	if (muted)
		EngineExternal->moduleAudio->ChangeRTPCValue(this->id, std::string("SourceVolume"), 0.0f);
	else
	{
		EngineExternal->moduleAudio->ChangeRTPCValue(this->id, std::string("SourceVolume"), this->volume);
		EngineExternal->moduleAudio->ChangeRTPCValue(this->id, std::string("SourcePitch"), this->pitch);
	}
	isMuted = muted;
}
