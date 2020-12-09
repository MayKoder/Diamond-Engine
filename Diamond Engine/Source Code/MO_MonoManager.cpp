#include "Globals.h"
#include "Application.h"
#include "MO_MonoManager.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/environment.h>
#include <mono/utils/mono-publib.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/object.h>
#include <mono/metadata/debug-helpers.h>

#pragma comment( lib, "mono-2.0-boehm.lib" )
#pragma comment( lib, "mono-2.0-sgen.lib" )

#pragma region Internals
//-------------------------------------------- Internals -----------------------------------------------//
void CSLog(MonoObject* x)
{
	MonoString* string = mono_object_to_string(x, NULL);
	LOG(LogType::L_WARNING, mono_string_to_utf8(string));
}

#include"MO_Input.h"
bool GetKey(MonoObject* x)
{
	if (EngineExternal != nullptr)
		return (EngineExternal->moduleInput->GetKey(*(int*)mono_object_unbox(x)) == KEY_DOWN);

	return false;
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

	MonoClass* klass = mono_object_get_class(position);
	float3 posVector;
	mono_field_get_value(position, mono_class_get_field_from_name(klass, "x"), &posVector.x);
	mono_field_get_value(position, mono_class_get_field_from_name(klass, "y"), &posVector.y);
	mono_field_get_value(position, mono_class_get_field_from_name(klass, "z"), &posVector.z);
	go->transform->position = posVector;
	go->transform->updateTransform = true;

	//MonoClass* klass;
	//MonoObject* obj;
	//klass = mono_class_from_name(EngineExternal->moduleMono->image, "Embed", "MyType");
	//if (!klass) {
	//	fprintf(stderr, "Can't find MyType in assembly %s\n", mono_image_get_filename(EngineExternal->moduleMono->image));
	//	exit(1);
	//}

	//obj = mono_object_new(EngineExternal->moduleMono->domain, klass);
	//mono_runtime_object_init(obj);
	//mono_field_set_value(obj, mono_class_get_field_from_name(klass, "reference"), &go);
}
#pragma endregion


M_MonoManager::M_MonoManager(Application* app, bool start_enabled) : Module(app, start_enabled), domain(nullptr),
object(nullptr), method(nullptr)
{

}

M_MonoManager::~M_MonoManager()
{}

// -----------------------------------------------------------------
bool M_MonoManager::Start()
{
	LOG(LogType::L_NORMAL, "Setting up the camera");
	bool ret = true;

	mono_config_parse(NULL);
	domain = mono_jit_init("CSTest/netcoreapp3.1/Assembly-CSharp.dll");

	mono_add_internal_call("MaykLogic.InternalCalls::CSLog", CSLog);
	mono_add_internal_call("MaykLogic.InternalCalls::GetKey", GetKey);
	mono_add_internal_call("MaykLogic.InternalCalls::CreateGameObject", CSCreateGameObject);

	MonoAssembly* assembly;
	assembly = mono_domain_assembly_open(domain, "CSTest/netcoreapp3.1/Assembly-CSharp.dll");
	if (!assembly)
		LOG(LogType::L_ERROR, "ERROR");

	MonoImage* image = mono_assembly_get_image(assembly);
	MonoClass* klass = mono_class_from_name(image, "MaykLogic", "MonoEmbed");
	object = mono_object_new(domain, klass);
	mono_runtime_object_init(object);

	void* iter = NULL;
	MonoMethod* method2;
	while (method2 = mono_class_get_methods(klass, &iter))
	{
		LOG(LogType::L_NORMAL, mono_method_full_name(method2, 1));
	}

	MonoMethodDesc* mdesc = mono_method_desc_new(":Update", false);
	method = mono_method_desc_search_in_class(mdesc, klass);

	return ret;
}

// -----------------------------------------------------------------
update_status M_MonoManager::Update(float dt)
{
	int x = 0;
	void* args[1];
	args[0] = &x;
	MonoObject* lengthObj = nullptr;
	if (x == 0)
	{
		lengthObj = mono_runtime_invoke(method, object, args, NULL);
		x = *(int*)mono_object_unbox(lengthObj);
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
bool M_MonoManager::CleanUp()
{
	LOG(LogType::L_NORMAL, "Cleaning mono domain");
	mono_jit_cleanup(domain); //Mono cleanup

	return true;
}