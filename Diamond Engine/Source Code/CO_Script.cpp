#include "CO_Script.h"
#include"ImGui/imgui.h"

#include"Application.h"
#include"MO_MonoManager.h"

#include <mono/metadata/class.h>

C_Script::C_Script(GameObject* _gm) : Component(_gm)
{
	name = "Script";

	EngineExternal->moduleMono->DebugAllFields("Core", fields);
	EngineExternal->moduleMono->DebugAllMethods("GameObject", methods);

}

C_Script::~C_Script()
{
	methods.clear();
	fields.clear();
}

void C_Script::Update()
{
}

bool C_Script::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		//ImGui::Separator();

		for (int i = 0; i < fields.size(); i++)
		{
			MonoType* type = mono_field_get_type(fields[i]);
			int a = *(int*)type;

			DropField(mono_field_get_name(fields[i]), "_GAMEOBJECT");
		}
		ImGui::Separator();
		for (int i = 0; i < methods.size(); i++)
		{
			ImGui::Text(methods[i].c_str());
		}

		return true;
	}
	return false;
}
