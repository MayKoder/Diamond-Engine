#include "Globals.h"
#include "Application.h"
#include "MO_MonoManager.h"
#include"MO_Renderer3D.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/class.h>
#include <mono/metadata/threads.h>

#include"GameObject.h"
#include"CO_Script.h"
#include"CS_Transform_Bindings.h"
#include "CS_Input_Bindings.h"
#include "CS_Audio_Bindings.h"

#include <iostream>
#include <fstream> 
#include"PugiXML/pugixml.hpp"
#include"IM_FileSystem.h"
#include"ImGui/imgui.h"
#include"WI_TextEditor.h"
#include"MO_Editor.h"

#pragma comment( lib, "mono/libx86/mono-2.0-boehm.lib" )
#pragma comment( lib, "mono/libx86/mono-2.0-sgen.lib" )

M_MonoManager::M_MonoManager(Application* app, bool start_enabled) : Module(app, start_enabled), domain(nullptr), domainThread(nullptr), assembly(nullptr), image(nullptr)
,jitDomain(nullptr)
{

}

M_MonoManager::~M_MonoManager()
{}

// -----------------------------------------------------------------
bool M_MonoManager::Init()
{
	LOG(LogType::L_NORMAL, "Setting up the camera");
	bool ret = true;

#ifndef STANDALONE
	CMDCompileCS();
#endif // !STANDALONE


	//mono_jit_set_aot_mode(MonoAotMode::MONO_AOT_MODE_HYBRID);
	mono_set_dirs("mono-runtime/lib", "mono-runtime/etc");
	mono_config_parse(NULL);
	jitDomain = mono_jit_init("myapp");

	mono_add_internal_call("DiamondEngine.Debug::Log", CSLog);
	mono_add_internal_call("DiamondEngine.Input::GetKey", GetKey);
	mono_add_internal_call("DiamondEngine.Input::GetMouseClick", GetMouseClick);
	mono_add_internal_call("DiamondEngine.InternalCalls::CreateGameObject", CSCreateGameObject);
	mono_add_internal_call("DiamondEngine.Input::GetMouseX", MouseX);
	mono_add_internal_call("DiamondEngine.Input::GetMouseY", MouseY);
	
	// --- Controller gamepad start --- //
	mono_add_internal_call("DiamondEngine.Input::GetGamepadButton", GetGamepadButton);
	//mono_add_internal_call("DiamondEngine.Input::GamepadAxis", GetGamepadAxis);
	mono_add_internal_call("DiamondEngine.Input::GetLeftTrigger", GetGamepadLeftTrigger);
	mono_add_internal_call("DiamondEngine.Input::GetRightTrigger", GetGamepadRightTrigger);
	mono_add_internal_call("DiamondEngine.Input::GetLeftAxisX", GetLeftAxisX);
	mono_add_internal_call("DiamondEngine.Input::GetLeftAxisY", GetLeftAxisY);
	mono_add_internal_call("DiamondEngine.Input::GetRightAxisY", GetRightAxisY);
	mono_add_internal_call("DiamondEngine.Input::GetRightAxisX", GetRightAxisX);
	// --- Controller gamepad end --- //

	mono_add_internal_call("DiamondEngine.InternalCalls::Destroy", Destroy);
	mono_add_internal_call("DiamondEngine.InternalCalls::CreateBullet", CreateBullet);

	mono_add_internal_call("DiamondEngine.GameObject::get_localPosition", SendPosition);
	mono_add_internal_call("DiamondEngine.GameObject::get_globalPosition", SendGlobalPosition);
	mono_add_internal_call("DiamondEngine.GameObject::set_localPosition", RecievePosition);

	mono_add_internal_call("DiamondEngine.GameObject::GetForward", GetForward);
	mono_add_internal_call("DiamondEngine.GameObject::GetRight", GetRight);

	mono_add_internal_call("DiamondEngine.GameObject::get_localRotation", SendRotation);
	mono_add_internal_call("DiamondEngine.GameObject::get_globalRotation", SendGlobalRotation);
	mono_add_internal_call("DiamondEngine.GameObject::set_localRotation", RecieveRotation);

	mono_add_internal_call("DiamondEngine.GameObject::get_localScale", SendScale);
	mono_add_internal_call("DiamondEngine.GameObject::get_globalScale", SendGlobalScale);
	mono_add_internal_call("DiamondEngine.GameObject::set_localScale", RecieveScale);

	mono_add_internal_call("DiamondEngine.Time::get_deltaTime", GetDT);

	mono_add_internal_call("DiamondEngine.Audio::PlayAudio", PlayAudio);
	mono_add_internal_call("DiamondEngine.Audio::StopAudio", StopAudio);
	mono_add_internal_call("DiamondEngine.Audio::ResumeAudio", ResumeAudio);
	mono_add_internal_call("DiamondEngine.Audio::PauseAudio", PauseAudio);
	mono_add_internal_call("DiamondEngine.Audio::GetVolume", GetVolume);
	mono_add_internal_call("DiamondEngine.Audio::SetVolume", SetVolume);
	mono_add_internal_call("DiamondEngine.Audio::GetPitch", GetPitch);
	mono_add_internal_call("DiamondEngine.Audio::SetPitch", SetPitch);
	mono_add_internal_call("DiamondEngine.Audio::GetMuted", GetMuted);
	mono_add_internal_call("DiamondEngine.Audio::SetMuted", SetMuted);

	InitMono();

	return ret;
}

// -----------------------------------------------------------------
bool M_MonoManager::CleanUp()
{
	LOG(LogType::L_NORMAL, "Cleaning mono domain");

	//mono_domain_unload(domain);
	mono_jit_cleanup(jitDomain); //Mono cleanup

	return true;
}

#ifndef STANDALONE
void M_MonoManager::OnGUI()
{
	if (ImGui::CollapsingHeader("Mono Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Compile tool: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "MSBuild portable version");
		
	}
}
#endif // !STANDALONE

void M_MonoManager::ReCompileCS() 
{
	if (DETime::state == GameState::PLAY)
		return;

	App->moduleScene->SaveScene("Library/Scenes/tmp.des");

	App->moduleScene->CleanScene();
	App->moduleRenderer3D->ClearAllRenderData();

	mono_domain_unload(domain);
	mono_thread_cleanup();

	while (mono_domain_is_unloading(domain) == true)
	{

	}

	CMDCompileCS();
	InitMono();

	App->moduleScene->LoadScene("Library/Scenes/tmp.des");
	App->moduleFileSystem->DeleteAssetFile("Library/Scenes/tmp.des"); //TODO: Duplicated code from editor, mmove to method

#ifndef STANDALONE
	W_TextEditor* txtEditor = dynamic_cast<W_TextEditor*>(App->moduleEditor->GetEditorWindow(EditorWindow::TEXTEDITOR));
	if (txtEditor != nullptr)
		txtEditor->SetTextFromFile(txtEditor->txtName.c_str());
#endif // !STANDALONE

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

void M_MonoManager::DebugAllFields(const char* className, std::vector<SerializedField>& _data, MonoObject* obj, C_Script* script)
{
	void* iter = NULL;
	MonoClassField* field;
	MonoClass* klass = mono_class_from_name(mono_assembly_get_image(EngineExternal->moduleMono->assembly), USER_SCRIPTS_NAMESPACE, className);
	while (field = mono_class_get_fields(klass, &iter))
	{
		SerializedField pushField = SerializedField(field, obj, script);



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
	MonoClass* goClass = mono_class_from_name(image, DE_SCRIPTS_NAMESPACE, "GameObject");
	uintptr_t goPtr = reinterpret_cast<uintptr_t>(inGo);

	void* args[2];
	args[0] = &inGo->name;
	args[1] = &goPtr;
	

	MonoMethodDesc* constructorDesc = mono_method_desc_new("DiamondEngine.GameObject:.ctor(string,uintptr)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constructorDesc, goClass);
	MonoObject* goObj = mono_object_new(domain, goClass);
	mono_runtime_invoke(method, goObj, args, NULL);

	mono_method_desc_free(constructorDesc);

	return goObj;
}

MonoObject* M_MonoManager::Float3ToCS(float3& inVec) const
{

	MonoClass* vecClass = mono_class_from_name(image, DE_SCRIPTS_NAMESPACE, "Vector3");

	MonoObject* vecObject = mono_object_new(domain, vecClass);
	const char* name = mono_class_get_name(mono_object_get_class(vecObject));

	void* args[3];
	args[0] = &inVec.x;
	args[1] = &inVec.y;
	args[2] = &inVec.z;
	
	MonoMethodDesc* constDesc = mono_method_desc_new("DiamondEngine.Vector3:.ctor(single,single,single)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constDesc, vecClass);

	mono_runtime_invoke(method, vecObject, args, NULL);

	mono_method_desc_free(constDesc);
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

	MonoClass* quadClass = mono_class_from_name(image, DE_SCRIPTS_NAMESPACE, "Quaternion");
	MonoObject* quatObject = mono_object_new(domain, quadClass);

	void* args[4];
	args[0] = &inVec.x;
	args[1] = &inVec.y;
	args[2] = &inVec.z;
	args[3] = &inVec.w;

	MonoMethodDesc* constDesc = mono_method_desc_new("DiamondEngine.Quaternion:.ctor(single,single,single,single)", true);
	MonoMethod* method = mono_method_desc_search_in_class(constDesc, quadClass);

	mono_runtime_invoke(method, quatObject, args, NULL);

	mono_method_desc_free(constDesc);
	return quatObject;
}

GameObject* M_MonoManager::GameObject_From_CSGO(MonoObject* goObj)
{
	uintptr_t ptr = 0;
	MonoClass* goClass = mono_class_from_name(image, DE_SCRIPTS_NAMESPACE, "GameObject");

	mono_field_get_value(goObj, mono_class_get_field_from_name(goClass, "pointer"), &ptr);

	return reinterpret_cast<GameObject*>(ptr);
}

SerializedField::SerializedField() : field(nullptr), parentSC(nullptr)
{
	fiValue.iValue = 0;
}

SerializedField::SerializedField(MonoClassField* _field, MonoObject* _object, C_Script* parent) : field(_field)
{
	type = static_cast<MonoTypeEnum>(mono_type_get_type(mono_field_get_type(field)));
	fiValue.iValue = 0;
	parentSC = parent;

	displayName += "##";
	displayName += mono_field_get_name(_field);

	M_MonoManager::LoadFieldData(*this, _object);
}

void M_MonoManager::CreateAssetsScript(const char* localPath) 
{
	std::string unnormalizedPath("Assets/");
	unnormalizedPath += localPath;
	unnormalizedPath = FileSystem::UnNormalizePath(unnormalizedPath.c_str());

	std::ofstream outfile(unnormalizedPath.c_str());

	std::string className("Assets/");
	className += localPath;
	className = className.substr(className.find_last_of("/") + 1);
	className = className.substr(0, className.find_last_of("."));

	outfile << "using System;" << std::endl << "using DiamondEngine;" << std::endl << std::endl << "public class " << className.c_str() << " : DiamondComponent" << std::endl << "{" << std::endl <<
		"	public void Update()" << std::endl << "	{" << std::endl << std::endl << "	}" << std::endl << std::endl << "}";

	outfile.close();

	AddScriptToSLN(unnormalizedPath.c_str());
	ReCompileCS();
}

void M_MonoManager::AddScriptToSLN(const char* scriptLocalPath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("Assembly-CSharp.csproj");

	if (result.status == pugi::xml_parse_status::status_file_not_found)
		assert(false, "XML File not loaded");

	std::string path; // Should be like ../Assets/Scripts/Hola.cs
	path += scriptLocalPath;
	std::string name = path.substr(path.find_last_of("\\"));

	pugi::xml_node whereToAdd = doc.child("Project");
	for (pugi::xml_node panel = whereToAdd.first_child(); panel != nullptr; panel = panel.next_sibling())
	{
		if (strcmp(panel.name(), "ItemGroup") == 0 && strcmp(panel.first_child().name(), "Compile") == 0)
		{
			panel = panel.append_child();
			panel.set_name("Compile");
			pugi::xml_attribute att = panel.append_attribute("Include");
			att.set_value(path.c_str());

			break;
		}
	}

	doc.save_file("Assembly-CSharp.csproj");
}

void M_MonoManager::RemoveScriptFromSLN(const char* scriptLocalPath)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("Assembly-CSharp.csproj");

	if (result.status == pugi::xml_parse_status::status_file_not_found)
		assert(false, "XML File not loaded");

	std::string path; // Should be like ../Assets/Scripts/Hola.cs

	pugi::xml_node whereToRemove = doc.child("Project");
	for (pugi::xml_node panel = whereToRemove.first_child(); panel != nullptr; panel = panel.next_sibling())
	{
		if (strcmp(panel.name(), "ItemGroup") == 0 && strcmp(panel.first_child().name(), "Compile") == 0)
		{
			for (pugi::xml_node toRemove = panel.first_child(); toRemove != nullptr; toRemove = toRemove.next_sibling())
			{
				path = FileSystem::NormalizePath(toRemove.attribute("Include").as_string());

				if (strcmp(path.c_str(), scriptLocalPath) == 0)
				{
					panel.remove_child(toRemove);
					break;
				}
			}
		}
	}

	doc.save_file("Assembly-CSharp.csproj");
}


void M_MonoManager::InitMono()
{
	//mono_set_dirs("mono-runtime/lib", "mono-runtime/etc");
	//mono_config_parse(NULL);

	domain = mono_domain_create_appdomain("D1", NULL);
	mono_domain_set(domain, 0);
	domainThread = mono_thread_attach(domain);

	//mono_thread_attach(domain);

	MonoImageOpenStatus sts;

	if (FileSystem::Exists("Library/ScriptsAssembly/Assembly-CSharp.dll") == false)
		assert(false && "You need to install the 'Visual Studio .NET desktop development pack' to run the engine :(");

	assembly = mono_assembly_open("Library/ScriptsAssembly/Assembly-CSharp.dll", &sts);
	//assembly = mono_domain_assembly_open(domain, "CSSolution/Assembly-CSharp/Build/Assembly-CSharp.dll");
	if (!assembly)
		LOG(LogType::L_ERROR, "ERROR");

	image = mono_assembly_get_image(assembly);



	const MonoTableInfo* table_info = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
	int rows = mono_table_info_get_rows(table_info);

	MonoClass* _class = nullptr;

	userScripts.clear();
	for (int i = 1; i < rows; i++)
	{
		uint32_t cols[MONO_TYPEDEF_SIZE];
		mono_metadata_decode_row(table_info, i, cols, MONO_TYPEDEF_SIZE);
		const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);
		const char* name_space = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
		_class = mono_class_from_name(image, name_space, name);

		if (strcmp(mono_class_get_namespace(_class), DE_SCRIPTS_NAMESPACE) != 0 && !mono_class_is_enum(_class))
		{
			userScripts.push_back(_class);
			LOG(LogType::L_WARNING, "%s", mono_class_get_name(_class));
		}
	}
}