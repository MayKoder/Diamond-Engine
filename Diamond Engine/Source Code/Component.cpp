#include "Component.h"
#include"DEJsonSupport.h"
#include"ImGui/imgui.h"
#include"GameObject.h"

Component::Component(GameObject* _gm) : active(true), /*name(""),*/ gameObject(_gm), type(TYPE::NONE)
{
}

Component::~Component()
{
}

void Component::Enable()
{
}

void Component::Disable()
{
}

void Component::Update()
{
}

void Component::PostUpdate()
{
}

#ifndef STANDALONE
bool Component::OnEditor()
{
	ImGui::Dummy(ImVec2(15, 10));
	bool ret = ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	if (!ret)
		return false;

	ImGui::PushID(&this->active);

	//ImGui::Text("UID: %i", gameObject->UID);

	if (this->type != Component::TYPE::TRANSFORM)
	{
		ImGui::Text("Active: "); ImGui::SameLine(); ImGui::Checkbox("##MeshActive", &active);

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - (ImGui::CalcTextSize("Remove").x * 1.2f));

		if (ImGui::Button("Remove"))
			gameObject->RemoveComponent(this);
	}
	ImGui::PopID();

	return true;
}
#endif // !STANDALONE

void Component::SaveData(JSON_Object* nObj)
{
	DEJson::WriteInt(nObj, "Type", (int)type);
	DEJson::WriteBool(nObj, "Active", active);
}

void Component::LoadData(DEConfig& nObj)
{
	active = nObj.ReadBool("Active");
}
