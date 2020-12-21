#include "CO_Script.h"
#include "ImGui/imgui.h"

#include "MO_Editor.h"
#include"MO_Scene.h"

#include "GameObject.h"
#include "Application.h"
#include "DETime.h"
#include "CO_Transform.h"

#include"DEJsonSupport.h"
#include <mono/metadata/class.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

C_Script* C_Script::runningScript = nullptr;
C_Script::C_Script(GameObject* _gm) : Component(_gm)
{
	name = "Script";


	EngineExternal->moduleMono->DebugAllMethods("DiamondEngine", "GameObject", methods);
	EngineExternal->moduleMono->DebugAllMethods("DiamondEngine", "Core", methods);
	//EngineExternal->moduleMono->DebugAllMethods("System", "Object", methods);

	MonoClass* klass = mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Core");
	
	coreObject = mono_object_new(EngineExternal->moduleMono->domain, klass);
	mono_runtime_object_init(coreObject);

	MonoMethodDesc* mdesc = mono_method_desc_new(":Update", false);
	updateMethod = mono_method_desc_search_in_class(mdesc, klass);
	mono_free(mdesc);

	EngineExternal->moduleMono->DebugAllFields("Core", fields, coreObject);
}

C_Script::~C_Script()
{
	methods.clear();
	fields.clear();
}

void C_Script::Update()
{
	if (DETime::state == GameState::STOP || DETime::state == GameState::PAUSE)
		return;

	C_Script::runningScript = this; // I really think this is the peak of stupid code, but hey, it works, slow as hell but works.

	//int x = 0;
	//void* args[1];
	//args[0] = &x;
	//MonoObject* lengthObj = nullptr;
	//if (x == 0)
	//{
		//BUG IMPORTANT: Core variables are like... shared? Maybe because the main? Rotation is affecting both scripts if you rotate +1 and -1 neither rotates
		/*lengthObj = */mono_runtime_invoke(updateMethod, coreObject, NULL, NULL); //TODO IMPORTANT: This is super slow, use managed chunks (mono documentation)
		//x = *(int*)mono_object_unbox(lengthObj);

		//for (int i = 0; i < fields.size(); i++) //TODO IMPORTANT: Better idea, add a callback on position, rotation or scale set{} in C# that calls an update on the c++ gameObject
		//{
		//	if (fields[i].type == MonoTypeEnum::MONO_TYPE_CLASS && fields[i].fiValue.goValue != nullptr) 
		//	{
		//		//TODO IMPORTANT: WE SHOULD USE TOKENS
		//		MonoClass* klass = mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Core");
		//		MonoClass* goClass = mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "GameObject");

		//		const char* test = mono_field_get_name(fields[i].field);
		//		MonoClassField* coreProperty = mono_class_get_field_from_name(klass, test);
		//		MonoObject* referenceObject = mono_field_get_value_object(EngineExternal->moduleMono->domain, coreProperty, coreObject);

		//		//MonoTypeEnum name = (MonoTypeEnum)mono_type_get_type(mono_field_get_type(mono_class_get_field_from_name(klass, "reference")));
		//		if (referenceObject)
		//		{
		//			MonoObject* vecObject = mono_object_new(EngineExternal->moduleMono->domain, mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Vector3"));
		//			mono_field_get_value(referenceObject, mono_class_get_field_from_name(goClass, "lPosition"), &vecObject);
		//			float3 csPosition = M_MonoManager::UnboxVector(vecObject);

		//			MonoObject* rotationObject = mono_object_new(EngineExternal->moduleMono->domain, mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Quaternion"));
		//			mono_field_get_value(referenceObject, mono_class_get_field_from_name(goClass, "lRotation"), &rotationObject);
		//			Quat csQuat = M_MonoManager::UnboxQuat(rotationObject);

		//			MonoObject* scaleObject = mono_object_new(EngineExternal->moduleMono->domain, mono_class_from_name(EngineExternal->moduleMono->image, "DiamondEngine", "Vector3"));
		//			mono_field_get_value(referenceObject, mono_class_get_field_from_name(goClass, "lScale"), &scaleObject);
		//			float3 csScale = M_MonoManager::UnboxVector(scaleObject);

		//			C_Transform* transform = fields[i].fiValue.goValue->transform; // TODO: This wont work because its hardcoded
		//			//LOG(LogType::L_WARNING, "C++ %f %f %f vs C# %f %f %f", transform->position.x, transform->position.y, transform->position.z, test.x, test.y, test.z);
		//			transform->SetTransformMatrix(csPosition, csQuat, csScale);
		//			transform->updateTransform = true;
		//		}
		//	}
		//}

	//}
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

void C_Script::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	for (int i = 0; i < fields.size(); i++)
	{
		switch (fields[i].type)
		{
		case MonoTypeEnum::MONO_TYPE_BOOLEAN:
			DEJson::WriteBool(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.bValue);
			break;

		case MonoTypeEnum::MONO_TYPE_I4:
			DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.iValue);
			break;

		case MonoTypeEnum::MONO_TYPE_CLASS:
			if(fields[i].fiValue.goValue != nullptr)
				DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.goValue->UID);
			break;

		case MonoTypeEnum::MONO_TYPE_R4:
			DEJson::WriteFloat(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.fValue);
			break;

		case MonoTypeEnum::MONO_TYPE_STRING:
			DEJson::WriteString(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.strValue);
			break;

		default:
			DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.iValue);
			break;
		}
	}
}
void C_Script::LoadData(JSON_Object* nObj)
{
	Component::LoadData(nObj);

	SerializedField* _field = nullptr;
	for (int i = 0; i < fields.size(); i++) //TODO IMPORTANT ASK: There must be a better way to do this... too much use of switches with this stuff, look at MONOMANAGER
	{
		_field = &fields[i];

		switch (_field->type)
		{
		case MonoTypeEnum::MONO_TYPE_BOOLEAN:
			_field->fiValue.bValue = DEJson::ReadBool(nObj, mono_field_get_name(_field->field));
			break;

		case MonoTypeEnum::MONO_TYPE_I4:
			_field->fiValue.iValue = DEJson::ReadInt(nObj, mono_field_get_name(_field->field));
			break;

		case MonoTypeEnum::MONO_TYPE_CLASS:
			//ERROR BUG IMPORTANT: Reference not working, if the object is a child, it wont be selected
			_field->fiValue.goValue = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, DEJson::ReadInt(nObj, mono_field_get_name(_field->field)));

			if(_field->fiValue.goValue)
				SetField(_field->field, _field->fiValue.goValue);

			break;

		case MonoTypeEnum::MONO_TYPE_R4:
			_field->fiValue.fValue = DEJson::ReadFloat(nObj, mono_field_get_name(_field->field));
			break;

		case MonoTypeEnum::MONO_TYPE_STRING: 
		{
			const char* ret = DEJson::ReadString(nObj, mono_field_get_name(_field->field));

			if (ret == NULL)
				ret = "\0";

			strcpy(&_field->fiValue.strValue[0], ret);

			break;
		}

		default:
			_field->fiValue.iValue = DEJson::ReadInt(nObj, mono_field_get_name(_field->field));
			break;
		}
	}
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
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), (field.fiValue.goValue != nullptr) ? field.fiValue.goValue->name.c_str() : "None");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(dropType))
			{
				field.fiValue.goValue = EngineExternal->moduleEditor->GetDraggingGO();
				SetField(field.field, field.fiValue.goValue);
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
