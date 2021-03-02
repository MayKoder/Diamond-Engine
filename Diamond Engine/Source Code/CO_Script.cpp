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
C_Script::C_Script(GameObject* _gm, const char* scriptName) : Component(_gm), noGCobject(0), updateMethod(nullptr)
{
	name = scriptName;
	//strcpy(name, scriptName);

	//EngineExternal->moduleMono->DebugAllMethods(DE_SCRIPTS_NAMESPACE, "GameObject", methods);
	LoadScriptData(scriptName);

	for (unsigned int i = 0; i < fields.size(); i++)
	{
		const char* name = mono_field_get_name(fields[i].field);
		if (strcmp(mono_field_get_name(fields[i].field), "thisReference") == 0) 
		{
			fields[i].fiValue.goValue = _gm;
			SetField(fields[i].field, _gm);
		}
	}

}

C_Script::~C_Script()
{
	if (C_Script::runningScript == this)
		C_Script::runningScript = nullptr;

	mono_gchandle_free(noGCobject);

	for (unsigned int i = 0; i < fields.size(); i++)
	{
		if (fields[i].type == MonoTypeEnum::MONO_TYPE_CLASS && fields[i].fiValue.goValue != nullptr && fields[i].fiValue.goValue->csReferences.size() != 0)
		{
			fields[i].fiValue.goValue->csReferences.erase(std::find(fields[i].fiValue.goValue->csReferences.begin(), fields[i].fiValue.goValue->csReferences.end(), &fields[i]));
		}
	}

	methods.clear();
	fields.clear();
	name.clear();
}

void C_Script::Update()
{
	if (DETime::state == GameState::STOP || DETime::state == GameState::PAUSE || updateMethod == nullptr)
		return;

	C_Script::runningScript = this; // I really think this is the peak of stupid code, but hey, it works, slow as hell but works.

	MonoObject* exec = nullptr;
	mono_runtime_invoke(updateMethod, mono_gchandle_get_target(noGCobject), NULL, &exec);

	if (exec != nullptr) 
	{
		if (strcmp(mono_class_get_name(mono_object_get_class(exec)), "NullReferenceException") == 0) 
		{
			LOG(LogType::L_ERROR, "Null reference exception detected");
		}
		else 
		{
			LOG(LogType::L_ERROR, "Something went wrong");
		}
	}
}

#ifndef STANDALONE
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
		mono_field_get_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.bValue);
		if (ImGui::Checkbox(field.displayName.c_str(), &field.fiValue.bValue))
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.bValue);
		break;

	case MonoTypeEnum::MONO_TYPE_I4:
		mono_field_get_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.iValue);
		if (ImGui::InputInt(field.displayName.c_str(), &field.fiValue.iValue, 1, 10))
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.iValue);
		break;

	case MonoTypeEnum::MONO_TYPE_CLASS:

		if (strcmp(mono_type_get_name(mono_field_get_type(field.field)), "DiamondEngine.GameObject") != 0)
		{
			ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "The class %s can't be serialized yet", mono_type_get_name(mono_field_get_type(field.field)));
			break;
		}

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

	case MonoTypeEnum::MONO_TYPE_R4: {
		//float test = 0.f;
		mono_field_get_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.fValue);
		if (ImGui::InputFloat(field.displayName.c_str(), &field.fiValue.fValue, 0.1f))
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, &field.fiValue.fValue);
		break;
	}

	case MonoTypeEnum::MONO_TYPE_STRING:
	{

		MonoString* str = nullptr;
		mono_field_get_value(mono_gchandle_get_target(noGCobject), field.field, &str);

		char* value = mono_string_to_utf8(str);
		strcpy(field.fiValue.strValue, value);
		mono_free(value);

		if (ImGui::InputText(field.displayName.c_str(), &field.fiValue.strValue[0], 50))
		{
			str = mono_string_new(EngineExternal->moduleMono->domain, field.fiValue.strValue);
			mono_field_set_value(mono_gchandle_get_target(noGCobject), field.field, str);
			//mono_free(str);
		}
		break;
	}

	default:
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), mono_type_get_name(mono_field_get_type(field.field)));
		break;
	}

	ImGui::PopID();
}
#endif

void C_Script::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);
	DEJson::WriteString(nObj, "ScriptName", name.c_str());

	for (int i = 0; i < fields.size(); i++)
	{
		switch (fields[i].type)
		{
		case MonoTypeEnum::MONO_TYPE_BOOLEAN:
			mono_field_get_value(mono_gchandle_get_target(noGCobject), fields[i].field, &fields[i].fiValue.bValue);
			DEJson::WriteBool(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.bValue);
			break;

		case MonoTypeEnum::MONO_TYPE_I4:
			mono_field_get_value(mono_gchandle_get_target(noGCobject), fields[i].field, &fields[i].fiValue.iValue);
			DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.iValue);
			break;

		case MonoTypeEnum::MONO_TYPE_CLASS:
			if(fields[i].fiValue.goValue != nullptr)
				DEJson::WriteInt(nObj, mono_field_get_name(fields[i].field), fields[i].fiValue.goValue->UID);
			break;

		case MonoTypeEnum::MONO_TYPE_R4:
			mono_field_get_value(mono_gchandle_get_target(noGCobject), fields[i].field, &fields[i].fiValue.fValue);
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
void C_Script::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	SerializedField* _field = nullptr;
	for (int i = 0; i < fields.size(); i++) //TODO IMPORTANT ASK: There must be a better way to do this... too much use of switches with this stuff, look at MONOMANAGER
	{
		_field = &fields[i];

		switch (_field->type)
		{
		case MonoTypeEnum::MONO_TYPE_BOOLEAN:
			_field->fiValue.bValue = nObj.ReadBool(mono_field_get_name(_field->field));
			mono_field_set_value(mono_gchandle_get_target(noGCobject), _field->field, &_field->fiValue.bValue);
			break;

		case MonoTypeEnum::MONO_TYPE_I4:
			_field->fiValue.iValue = nObj.ReadInt(mono_field_get_name(_field->field));
			mono_field_set_value(mono_gchandle_get_target(noGCobject), _field->field, &_field->fiValue.iValue);
			break;

		case MonoTypeEnum::MONO_TYPE_CLASS:
		{
			if (strcmp(mono_type_get_name(mono_field_get_type(_field->field)), "DiamondEngine.GameObject") == 0)
				EngineExternal->moduleScene->referenceMap.emplace(nObj.ReadInt(mono_field_get_name(_field->field)), _field);

			break;
		}
		case MonoTypeEnum::MONO_TYPE_R4:
			_field->fiValue.fValue = nObj.ReadFloat(mono_field_get_name(_field->field));
			mono_field_set_value(mono_gchandle_get_target(noGCobject), _field->field, &_field->fiValue.fValue);
			break;

		case MonoTypeEnum::MONO_TYPE_STRING: 
		{
			const char* ret = nObj.ReadString(mono_field_get_name(_field->field));

			if (ret == NULL)
				ret = "\0";

			strcpy(&_field->fiValue.strValue[0], ret);

			MonoString* str = mono_string_new(EngineExternal->moduleMono->domain, _field->fiValue.strValue);
			mono_field_set_value(mono_gchandle_get_target(noGCobject), _field->field, str);
			break;
		}

		default:
			_field->fiValue.iValue = nObj.ReadInt(mono_field_get_name(_field->field));
			mono_field_set_value(mono_gchandle_get_target(noGCobject), _field->field, &_field->fiValue.iValue);
			break;
		}
	}
}

void C_Script::LoadScriptData(const char* scriptName)
{
	methods.clear();
	fields.clear();


	MonoClass* klass = mono_class_from_name(EngineExternal->moduleMono->image, USER_SCRIPTS_NAMESPACE, scriptName);

	if (klass == nullptr) 
	{
		LOG(LogType::L_ERROR, "Script %s was deleted and can't be loaded", scriptName);
		name = "Missing script reference";
		return;
	}

	EngineExternal->moduleMono->DebugAllMethods(USER_SCRIPTS_NAMESPACE, scriptName, methods);

	noGCobject = mono_gchandle_new(mono_object_new(EngineExternal->moduleMono->domain, klass), false);
	mono_runtime_object_init(mono_gchandle_get_target(noGCobject));

	MonoMethodDesc* mdesc = mono_method_desc_new(":Update", false);
	updateMethod = mono_method_desc_search_in_class(mdesc, klass);
	mono_method_desc_free(mdesc);

	MonoMethodDesc* oncDesc = mono_method_desc_new(":OnCollisionEnter", false);
	onCollisionEnter = mono_method_desc_search_in_class(oncDesc, klass);
	mono_method_desc_free(oncDesc);


	EngineExternal->moduleMono->DebugAllFields(scriptName, fields, mono_gchandle_get_target(noGCobject), this);
}

void C_Script::CollisionCallback()
{
	if(onCollisionEnter != nullptr)
		mono_runtime_invoke(onCollisionEnter, mono_gchandle_get_target(noGCobject), NULL, NULL);
}

void C_Script::SetField(MonoClassField* field, GameObject* value)
{
	mono_field_set_value(mono_gchandle_get_target(noGCobject), field, EngineExternal->moduleMono->GoToCSGO(value));
}
