#include "Component.h"
#include"DEJsonSupport.h"
#include"ImGui/imgui.h"
#include"GameObject.h"

Component::Component(GameObject* _gm) : active(true), /*name(""),*/ gameObject(_gm), type(Type::None), name(nullptr)
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

bool Component::OnEditor()
{
	bool ret = ImGui::CollapsingHeader(name, ImGuiTreeNodeFlags_DefaultOpen);

	if (!ret)
		return false;

	ImGui::PushID(&this->active);
	if (this->type != Component::Type::Transform) 
	{
		ImGui::Text("Active: "); ImGui::SameLine(); ImGui::Checkbox("##MeshActive", &active);

		ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - (ImGui::CalcTextSize("Remove").x * 1.2f));

		if (ImGui::Button("Remove"))
			gameObject->RemoveComponent(this);
	}
	ImGui::PopID();

	return true;
}

void Component::SaveData(JSON_Object* nObj)
{
	DEJson::WriteInt(nObj, "Type", (int)type);
	DEJson::WriteBool(nObj, "Active", active);
}

void Component::LoadData(JSON_Object* nObj)
{
	active = DEJson::ReadBool(nObj, "Active");
}
