#include "CO_ParticleSystem.h"
#include "ImGui/imgui.h"
#include "Application.h"

C_ParticleSystem::C_ParticleSystem(GameObject* _gm) : Component(_gm), systemActive(true),myEmitters()
{
	name = "Particle System";
}

C_ParticleSystem::~C_ParticleSystem()
{
	systemActive = false;

}

#ifndef STANDALONE
bool C_ParticleSystem::OnEditor()
{

	if (Component::OnEditor() == false)
		return false;
	ImGui::Separator();
	ImGui::Checkbox("SystemActive", &systemActive);

	ImGui::Spacing();
	std::string guiName = "";
	std::string suffixLabel = "";
	for (int i = 0; i < myEmitters.size(); ++i)
	{
		suffixLabel = "##";
		suffixLabel += i;
		ImGui::Separator();
		ImGui::Spacing();

		myEmitters[i].OnEditor(i);

		guiName = "Delete Emitter"+suffixLabel;
		if(ImGui::Button(guiName.c_str()))
		{
			myEmitters[i].toDelete = true;
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (ImGui::Button("Add Emitter"))
	{
		myEmitters.push_back(Emitter());
	}
	return true;
}
#endif // !STANDALONE

void C_ParticleSystem::Update()
{
	float dt = EngineExternal->GetDT();
	//delete emitters
	for (int i = myEmitters.size()-1; i >= 0; --i)
	{
		if (myEmitters[i].toDelete)
		{
			myEmitters.erase(myEmitters.begin() + i);
		}
	}


	for (int i = 0; i < myEmitters.size(); ++i)
	{
		myEmitters[i].Update(dt,systemActive);
	}
}

void C_ParticleSystem::Draw()
{
	for (int i = 0; i < myEmitters.size(); ++i)
	{
		myEmitters[i].Draw();
	}
}

void C_ParticleSystem::SetActive(bool newActive)
{
	systemActive = newActive;
}

void C_ParticleSystem::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

}

void C_ParticleSystem::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

}
