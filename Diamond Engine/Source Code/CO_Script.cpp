#include "CO_Script.h"
#include "ImGui/imgui.h"

#include "MO_Editor.h"

#include "GameObject.h"
#include "Application.h"
#include "DETime.h"
#include "CO_Transform.h"

#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

C_Script::C_Script(GameObject* _gm) : Component(_gm), temporalReference(nullptr)
{
	name = "Script";

	EngineExternal->moduleMono->DebugAllFields("Core", fields);
	EngineExternal->moduleMono->DebugAllMethods("GameObject", methods);

	MonoClass* klass = mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Core");
	coreObject = mono_object_new(EngineExternal->moduleMono->domain, klass);
	mono_runtime_object_init(coreObject);

	MonoMethodDesc* mdesc = mono_method_desc_new(":Update", false);
	updateMethod = mono_method_desc_search_in_class(mdesc, klass);
	mono_free(mdesc);
}

C_Script::~C_Script()
{
	methods.clear();
	fields.clear();
}

void C_Script::Update()
{
	if (DETime::state != GameState::PLAY)
		return;

	int x = 0;
	void* args[1];
	args[0] = &x;
	MonoObject* lengthObj = nullptr;
	if (x == 0)
	{
		lengthObj = mono_runtime_invoke(updateMethod, coreObject, args, NULL); //TODO IMPORTANT: This is super slow, use managed chunks (mono documentation)
		x = *(int*)mono_object_unbox(lengthObj);

		//TODO IMPORTANT: WE SHOULD USE TOKENS
		MonoClass* klass = mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Core");
		MonoClass* goClass = mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "GameObject");
		MonoClassField* coreProperty = mono_class_get_field_from_name(klass, "reference");
		MonoObject* referenceObject = mono_field_get_value_object(EngineExternal->moduleMono->domain, coreProperty, coreObject);

		//MonoTypeEnum name = (MonoTypeEnum)mono_type_get_type(mono_field_get_type(mono_class_get_field_from_name(klass, "reference")));
		if (referenceObject && temporalReference)
		{

			MonoObject* vecObject = mono_object_new(EngineExternal->moduleMono->domain, mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Vector3"));
			mono_field_get_value(referenceObject, mono_class_get_field_from_name(goClass, "position"), &vecObject);
			float3 csPosition = M_MonoManager::UnboxVector(vecObject);

			MonoObject* rotationObject = mono_object_new(EngineExternal->moduleMono->domain, mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Quaternion"));
			mono_field_get_value(referenceObject, mono_class_get_field_from_name(goClass, "rotation"), &rotationObject);
			Quat csQuat = M_MonoManager::UnboxQuat(rotationObject);

			MonoObject* scaleObject = mono_object_new(EngineExternal->moduleMono->domain, mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Vector3"));
			mono_field_get_value(referenceObject, mono_class_get_field_from_name(goClass, "scale"), &scaleObject);
			float3 csScale = M_MonoManager::UnboxVector(scaleObject);

			C_Transform* transform = temporalReference->transform; // TODO: This wont work because its hardcoded
			//LOG(LogType::L_WARNING, "C++ %f %f %f vs C# %f %f %f", transform->position.x, transform->position.y, transform->position.z, test.x, test.y, test.z);
			transform->SetTransformMatrix(csPosition, csQuat, csScale);
			transform->updateTransform = true;
		}
	}
}

bool C_Script::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		//ImGui::Separator();

		for (int i = 0; i < fields.size(); i++)
		{
			DropField(fields[i], "_GAMEOBJECT");
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

void C_Script::DropField(SerializedField& field, const char* dropType)
{

	const char* fieldName = mono_field_get_name(field.field);
	ImGui::PushID(fieldName);

	ImGui::Text(fieldName);
	ImGui::SameLine();


	switch (field.type)
	{
	case MonoTypeEnum::MONO_TYPE_BOOLEAN:
		if(ImGui::Checkbox(mono_field_get_name(field.field), &field.fiValue.bValue))
			mono_field_set_value(coreObject, field.field, &field.fiValue.bValue);
		break;

	case MonoTypeEnum::MONO_TYPE_I4:
		if(ImGui::InputInt(mono_field_get_name(field.field), &field.fiValue.iValue, 1, 10))
			mono_field_set_value(coreObject, field.field, &field.fiValue.iValue);
		break;

	case MonoTypeEnum::MONO_TYPE_CLASS:
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "This is a custom class field");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dropType))
			{
				temporalReference = EngineExternal->moduleEditor->GetSelectedGO();
				SetField(field.field, temporalReference);
			}
			ImGui::EndDragDropTarget();
		}
		break;

	case MonoTypeEnum::MONO_TYPE_R4:
		if(ImGui::InputFloat(mono_field_get_name(field.field), &field.fiValue.fValue, 0.1f))
			mono_field_set_value(coreObject, field.field, &field.fiValue.fValue);
		break;

	case MonoTypeEnum::MONO_TYPE_STRING:
		if(ImGui::InputText(mono_field_get_name(field.field), &field.fiValue.strValue[0], 50))
			mono_field_set_value(coreObject, field.field, mono_string_new(EngineExternal->moduleMono->domain, field.fiValue.strValue));
		break;

	default:
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), mono_type_get_name(mono_field_get_type(field.field)));
		break;
	}
	//TODO: Update C# field value

	ImGui::PopID();
}

void C_Script::SetField(MonoClassField* field, GameObject* value)
{
	mono_field_set_value(coreObject, field, EngineExternal->moduleMono->GoToCSGO(value));
}
