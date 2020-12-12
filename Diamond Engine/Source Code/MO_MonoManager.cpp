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
bool GetKey(MonoObject* x)
{
	if (EngineExternal != nullptr)
		return (EngineExternal->moduleInput->GetKey(*(int*)mono_object_unbox(x)) == KEY_REPEAT);

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
#pragma endregion


M_MonoManager::M_MonoManager(Application* app, bool start_enabled) : Module(app, start_enabled), domain(nullptr),
coreObject(nullptr), updateMethod(nullptr), assembly(nullptr), image(nullptr)	
{

}

M_MonoManager::~M_MonoManager()
{}

// -----------------------------------------------------------------
bool M_MonoManager::Start()
{
	LOG(LogType::L_NORMAL, "Setting up the camera");
	bool ret = true;

	mono_set_dirs("mono-runtime/lib", "mono-runtime/etc");

	mono_config_parse(NULL);
	domain = mono_jit_init("CSSolution/Assembly-CSharp/Build/Assembly-CSharp.dll");

	mono_add_internal_call("DiamondEngine.InternalCalls::CSLog", CSLog);
	mono_add_internal_call("DiamondEngine.InternalCalls::GetKey", GetKey);
	mono_add_internal_call("DiamondEngine.InternalCalls::CreateGameObject", CSCreateGameObject);

	assembly = mono_domain_assembly_open(domain, "CSSolution/Assembly-CSharp/Build/Assembly-CSharp.dll");
	if (!assembly)
		LOG(LogType::L_ERROR, "ERROR");

	image = mono_assembly_get_image(assembly);
	MonoClass* klass = mono_class_from_name(image, "DiamondEngine", "Core");
	coreObject = mono_object_new(domain, klass);
	mono_runtime_object_init(coreObject);


	MonoMethodDesc* mdesc = mono_method_desc_new(":Update", false);
	updateMethod = mono_method_desc_search_in_class(mdesc, klass);

	//DebugAllFields("GameObject");

	MonoClass* coreClass = mono_class_from_name(image, "DiamondEngine", "Core");
	MonoClassField* coreProperty = mono_class_get_field_from_name(klass, "reference");
	mono_field_set_value(coreObject, coreProperty, GoToCSGO(App->moduleScene->root->children[0]));

	App->moduleScene->root->children[0]->AddComponent(Component::Type::Script);

	mono_free(mdesc);

	return ret;
}

// -----------------------------------------------------------------
update_status M_MonoManager::Update(float dt)
{
	if (DETime::state != GameState::PLAY)
		return update_status::UPDATE_CONTINUE;

	int x = 0;
	void* args[1];
	args[0] = &x;
	MonoObject* lengthObj = nullptr;
	if (x == 0)
	{
		lengthObj = mono_runtime_invoke(updateMethod, coreObject, args, NULL); //TODO IMPORTANT: This is super slow, use managed chunks (mono documentation)
		x = *(int*)mono_object_unbox(lengthObj);

		//TODO IMPORTANT: WE SHOULD USE TOKENS
		MonoClass* klass = mono_class_from_name(image, "DiamondEngine", "Core");
		MonoClass* goClass = mono_class_from_name(image, "DiamondEngine", "GameObject");
		MonoClassField* coreProperty = mono_class_get_field_from_name(klass, "reference");
		MonoObject* referenceObject = mono_field_get_value_object(domain, coreProperty, coreObject);

		MonoObject* vecObject = mono_object_new(domain, mono_class_from_name(image, "DiamondEngine", "Vector3"));
		mono_field_get_value(referenceObject, mono_class_get_field_from_name(goClass, "position"), &vecObject);
		float3 test = UnboxVector(vecObject);

		C_Transform* transform = App->moduleScene->root->children[0]->transform;
		//LOG(LogType::L_WARNING, "C++ %f %f %f vs C# %f %f %f", transform->position.x, transform->position.y, transform->position.z, test.x, test.y, test.z);
		transform->SetTransformMatrix(test, transform->rotation, transform->localScale);
		transform->updateTransform = true;
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

void M_MonoManager::DebugAllFields(const char* className, std::vector<MonoClassField*>& _data)
{
	void* iter = NULL;
	MonoClassField* field;
	MonoClass* klass = mono_class_from_name(mono_assembly_get_image(EngineExternal->moduleMono->assembly), "DiamondEngine", className);
	while (field = mono_class_get_fields(klass, &iter))
	{
		_data.push_back(field);
		//LOG(LogType::L_NORMAL, mono_field_full_name(method2));
	}
}

void M_MonoManager::DebugAllMethods(const char* className, std::vector<std::string>& _data)
{
	void* iter = NULL;
	MonoMethod* method2;
	MonoClass* klass = mono_class_from_name(mono_assembly_get_image(EngineExternal->moduleMono->assembly), "DiamondEngine", className);
	while (method2 = mono_class_get_methods(klass, &iter))
	{
		_data.push_back(mono_method_full_name(method2, 1));
		//LOG(LogType::L_NORMAL, mono_method_full_name(method2, 1));
	}
}

MonoObject* M_MonoManager::GoToCSGO(GameObject* inGo) const
{
	MonoClass* goClass = mono_class_from_name(image, "DiamondEngine", "GameObject");

	void* args[2];
	args[0] = mono_string_new(domain, "Mono rocks");
	args[1] = Float3ToCS(inGo->transform->position);
	
	//DebugAllMethods("GameObject");
	
	MonoMethodDesc* test = mono_method_desc_new("DiamondEngine.GameObject:.ctor (string,DiamondEngine.Vector3)", true);
	MonoMethod* method = mono_method_desc_search_in_class(test, goClass);
	MonoObject* goObj = mono_object_new(domain, goClass);
	mono_runtime_invoke(method, goObj, args, NULL);

	return goObj;
}

MonoObject* M_MonoManager::Float3ToCS(float3& inVec) const
{

	MonoClass* vecClass = mono_class_from_name(image, "DiamondEngine", "Vector3");
	MonoObject* vecObject = mono_object_new(domain, vecClass);
	
	mono_field_set_value(vecObject, mono_class_get_field_from_name(vecClass, "x"), &inVec.x);
	mono_field_set_value(vecObject, mono_class_get_field_from_name(vecClass, "y"), &inVec.y);
	mono_field_set_value(vecObject, mono_class_get_field_from_name(vecClass, "z"), &inVec.z);

	return vecObject;
}
