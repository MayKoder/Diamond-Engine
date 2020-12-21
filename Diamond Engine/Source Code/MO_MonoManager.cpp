#include "Globals.h"
#include "Application.h"
#include "MO_MonoManager.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/class.h>

#include"GameObject.h"
#include"DETime.h"
#include"CO_Script.h"

#pragma comment( lib, "mono/libx86/mono-2.0-boehm.lib" )
#pragma comment( lib, "mono/libx86/mono-2.0-sgen.lib" )

#pragma region Internals
//-------------------------------------------- Internals -----------------------------------------------//
void CSLog(MonoObject* x)
{
	if (x == NULL)
		return;

	MonoString* string = mono_object_to_string(x, NULL);
	LOG(LogType::L_WARNING, mono_string_to_utf8(string));
}

#include"MO_Input.h"
int GetKey(MonoObject* x)
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetKey(*(int*)mono_object_unbox(x));

	return false;
}
int MouseX()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetMouseXMotion();
}
int MouseY()
{
	if (EngineExternal != nullptr)
		return EngineExternal->moduleInput->GetMouseYMotion();
}

#include"MO_Scene.h"
#include"GameObject.h"
#include"MathGeoLib/include/Math/float3.h"
#include"CO_Transform.h"
void CSCreateGameObject(MonoObject* name, MonoObject* position)
{
	if (EngineExternal == nullptr)
		return;

	char* p = mono_string_to_utf8(mono_object_to_string(name, NULL));
	GameObject* go = EngineExternal->moduleScene->CreateGameObject(p, EngineExternal->moduleScene->root);
	mono_free(p);

	float3 posVector = M_MonoManager::UnboxVector(position);

	go->transform->position = posVector;
	go->transform->updateTransform = true;

	//MonoImage* image = mono_assembly_get_image(EngineExternal->moduleMono->assembly);
	//MonoClass* klass;
	//MonoObject* obj;
	//klass = mono_class_from_name(image, "DiamondEngine", "Core");
	//if (!klass) {
	//	fprintf(stderr, "Can't find MyType in assembly %s\n", mono_image_get_filename(image));
	//	exit(1);
	//}
	//obj = mono_object_new(EngineExternal->moduleMono->domain, klass);
	//mono_runtime_object_init(obj);
	//mono_field_set_value(obj, mono_class_get_field_from_name(klass, "reference"), go);
}

void UpdateCppPosition(int _UID, MonoObject* position)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID);
	if (workGO == nullptr)
		return;


	float3 newPos = EngineExternal->moduleMono->UnboxVector(position);

	workGO->transform->SetTransformMatrix(newPos, workGO->transform->rotation, workGO->transform->localScale);
	workGO->transform->updateTransform = true;

}
void UpdateCppRotation(int _UID, MonoObject* rotation)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID);
	if (workGO == nullptr)
		return;

	Quat newRot = EngineExternal->moduleMono->UnboxQuat(rotation);

	workGO->transform->SetTransformMatrix(workGO->transform->position, newRot, workGO->transform->localScale);
	workGO->transform->updateTransform = true;

}
void UpdateCppScale(int _UID, MonoObject* scale)
{
	if (EngineExternal == nullptr)
		return;

	GameObject* workGO = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, _UID);
	if (workGO == nullptr)
		return;


	float3 newScale = EngineExternal->moduleMono->UnboxVector(scale);

	workGO->transform->SetTransformMatrix(workGO->transform->position, workGO->transform->rotation, newScale);
	workGO->transform->updateTransform = true;

}

MonoObject* SendPosition()
{
	if (EngineExternal == nullptr)
		return nullptr;

	return EngineExternal->moduleMono->Float3ToCS(C_Script::runningScript->GetGO()->transform->position);
}

#pragma endregion


M_MonoManager::M_MonoManager(Application* app, bool start_enabled) : Module(app, start_enabled), domain(nullptr), updateMethod(nullptr), assembly(nullptr), image(nullptr)	
{
	mono_set_dirs("mono-runtime/lib", "mono-runtime/etc");

	mono_config_parse(NULL);
	domain = mono_jit_init("CSSolution/Assembly-CSharp/Build/Assembly-CSharp.dll");

	mono_add_internal_call("DiamondEngine.Debug::Log", CSLog);
	mono_add_internal_call("DiamondEngine.InternalCalls::GetKey", GetKey);
	mono_add_internal_call("DiamondEngine.InternalCalls::CreateGameObject", CSCreateGameObject);
	mono_add_internal_call("DiamondEngine.InternalCalls::GetMouseX", MouseX);
	mono_add_internal_call("DiamondEngine.InternalCalls::GetMouseY", MouseY);
	//mono_add_internal_call("DiamondEngine.InternalCalls::UpdateCppGO", UpdateTransformFromCS);
	mono_add_internal_call("DiamondEngine.InternalCalls::UpdateCppPosition", UpdateCppPosition);
	mono_add_internal_call("DiamondEngine.InternalCalls::UpdateCppRotation", UpdateCppRotation);
	mono_add_internal_call("DiamondEngine.InternalCalls::UpdateCppScale", UpdateCppScale);

	mono_add_internal_call("DiamondEngine.GameObject::get_testPosition", SendPosition);

	assembly = mono_domain_assembly_open(domain, "CSSolution/Assembly-CSharp/Build/Assembly-CSharp.dll");
	if (!assembly)
		LOG(LogType::L_ERROR, "ERROR");

	image = mono_assembly_get_image(assembly);
}

M_MonoManager::~M_MonoManager()
{}

// -----------------------------------------------------------------
bool M_MonoManager::Init()
{
	LOG(LogType::L_NORMAL, "Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool M_MonoManager::CleanUp()
{
	LOG(LogType::L_NORMAL, "Cleaning mono domain");
	mono_jit_cleanup(domain); //Mono cleanup

	return true;
}

//ASK: Is this the worst idea ever? TOO SLOW
float3 M_MonoManager::UnboxVector(MonoObject* _obj)
{
	float3 ret;

	MonoClass* klass = mono_object_get_class(_obj);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "x"), &ret.x);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "y"), &ret.y);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "z"), &ret.z);
	return ret;
}
//ASK: Is this the worst idea ever? TOO SLOW
Quat M_MonoManager::UnboxQuat(MonoObject* _obj)
{
	Quat ret;

	MonoClass* klass = mono_object_get_class(_obj);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "x"), &ret.x);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "y"), &ret.y);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "z"), &ret.z);
	mono_field_get_value(_obj, mono_class_get_field_from_name(klass, "w"), &ret.w);
	return ret;
}

void M_MonoManager::DebugAllFields(const char* className, std::vector<SerializedField>& _data, MonoObject* obj)
{
	void* iter = NULL;
	MonoClassField* field;
	MonoClass* klass = mono_class_from_name(mono_assembly_get_image(EngineExternal->moduleMono->assembly), "DiamondEngine", className);
	while (field = mono_class_get_fields(klass, &iter))
	{
		SerializedField pushField = SerializedField(field, obj);



		_data.push_back(pushField);
		//LOG(LogType::L_NORMAL, mono_field_full_name(method2));
	}
}

void M_MonoManager::DebugAllMethods(const char* nsName, const char* className, std::vector<std::string>& _data)
{
	void* iter = NULL;
	MonoMethod* method2;
	MonoClass* klass = mono_class_from_name(mono_assembly_get_image(EngineExternal->moduleMono->assembly), nsName, className);
	while (method2 = mono_class_get_methods(klass, &iter))
	{
		_data.push_back(mono_method_full_name(method2, 1));
		//LOG(LogType::L_NORMAL, mono_method_full_name(method2, 1));
	}
}

MonoObject* M_MonoManager::GoToCSGO(GameObject* inGo) const
{
	MonoClass* goClass = mono_class_from_name(image, "DiamondEngine", "GameObject");

	void* args[5];
	args[0] = mono_string_new(domain, inGo->name.c_str());
	args[1] = Float3ToCS(inGo->transform->position);
	args[2] = QuatToCS(inGo->transform->rotation);
	args[3] = Float3ToCS(inGo->transform->localScale);
	args[4] = &inGo->UID;
	
	//DebugAllMethods("GameObject");
	MonoMethodDesc* constructorDesc = mono_method_desc_new("DiamondEngine.GameObject:.ctor(string,DiamondEngine.Vector3,DiamondEngine.Quaternion,DiamondEngine.Vector3,int)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, goClass);
	MonoObject* goObj = mono_object_new(domain, goClass);
	mono_runtime_invoke(method, goObj, args, NULL);

	return goObj;
}

MonoObject* M_MonoManager::Float3ToCS(float3& inVec) const
{

	MonoClass* vecClass = mono_class_from_name(image, "DiamondEngine", "Vector3");

	MonoObject* vecObject = mono_object_new(domain, vecClass);

	void* args[3];
	args[0] = &inVec.x;
	args[1] = &inVec.y;
	args[2] = &inVec.z;
	
	MonoMethodDesc* constDesc = mono_method_desc_new("DiamondEngine.Vector3:.ctor(single,single,single)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constDesc, vecClass);

	mono_runtime_invoke(method, vecObject, args, NULL);

	return vecObject;
}

void M_MonoManager::LoadFieldData(SerializedField& _field, MonoObject* _object)
{
	switch (_field.type)
	{
	case MonoTypeEnum::MONO_TYPE_BOOLEAN:
		mono_field_get_value(_object, _field.field, &_field.fiValue.bValue);
		break;

	case MonoTypeEnum::MONO_TYPE_I4:
		mono_field_get_value(_object, _field.field, &_field.fiValue.iValue);
		break;

	case MonoTypeEnum::MONO_TYPE_CLASS:
		_field.fiValue.goValue = nullptr;
		break;

	case MonoTypeEnum::MONO_TYPE_R4:
		mono_field_get_value(_object, _field.field, &_field.fiValue.fValue);
		break;

	case MonoTypeEnum::MONO_TYPE_STRING:
		mono_field_get_value(_object, _field.field, &_field.fiValue.strValue);
		break;

	default:
		mono_field_get_value(_object, _field.field, &_field.fiValue.iValue);
		break;
	}
}

MonoObject* M_MonoManager::QuatToCS(Quat& inVec) const
{

	MonoClass* quadClass = mono_class_from_name(image, "DiamondEngine", "Quaternion");
	MonoObject* quatObject = mono_object_new(domain, quadClass);

	void* args[4];
	args[0] = &inVec.x;
	args[1] = &inVec.y;
	args[2] = &inVec.z;
	args[3] = &inVec.w;

	MonoMethodDesc* constDesc = mono_method_desc_new("DiamondEngine.Quaternion:.ctor(single,single,single,single)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constDesc, quadClass);

	mono_runtime_invoke(method, quatObject, args, NULL);

	return quatObject;
}

SerializedField::SerializedField() : field(nullptr)
{
	fiValue.iValue = 0;
}

SerializedField::SerializedField(MonoClassField* _field, MonoObject* _object)
{
	field = _field;
	type = static_cast<MonoTypeEnum>(mono_type_get_type(mono_field_get_type(field)));
	fiValue.iValue = 0;

	M_MonoManager::LoadFieldData(*this, _object);
}
