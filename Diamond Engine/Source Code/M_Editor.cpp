#include "Globals.h"
#include "Application.h"
#include "M_Editor.h"
#include "MaykMath.h"

#include "MMGui.h"
#include "parson/parson.h"

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "FileSystem.h"

//Window types
#include "W_Configuration.h"
#include "W_Console.h"
#include "W_About.h"
#include "W_Inspector.h"
#include "W_Hierarchy.h"
#include "W_Scene.h"
#include "W_Assets.h"

//#include"MathGeoLib/include/Math/float3.h"


M_Editor::M_Editor(Application* app, bool start_enabled) : Module(app, start_enabled), displayWindow(false),
viewportCorSize(0.f), dockspace_id(0)
{
	//reserve() does not work with [] operator
	windows = std::vector<Window*>(static_cast<unsigned int>(EditorWindow::MAX), nullptr);

	windows[static_cast<unsigned int>(EditorWindow::ASSETS)] = new W_Assets();
	windows[static_cast<unsigned int>(EditorWindow::CONSOLE)] = new W_Console();
	windows[static_cast<unsigned int>(EditorWindow::HIERARCHY)] = new W_Hierarchy(App->moduleScene);
	windows[static_cast<unsigned int>(EditorWindow::INSPECTOR)] = new W_Inspector();
	windows[static_cast<unsigned int>(EditorWindow::SCENE)] = new W_Scene(App);

	//TODO: This 2 windows are last on the enum to keep them from drawing on the window
	//tab on the main menu bar, and are drawed by hand on other tabs, there
	//must be a better way to do that
	windows[static_cast<unsigned int>(EditorWindow::ABOUT)] = new W_About();
	windows[static_cast<unsigned int>(EditorWindow::CONFIGURATION)] = new W_Configuration();

	//Sould load the last used style on start?
	UpdateLoadedStylesVector(&styles);

}

M_Editor::~M_Editor()
{
}

bool M_Editor::Init()
{

	LOG(LogType::L_NORMAL, "Init: ImGui");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	//io.ConfigViewportsNoDecoration = true;                    
	//io.ConfigViewportsNoAutoMerge = true;

	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowMenuButtonPosition = ImGuiDir_None;
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.219f, 0.219f, 0.219f, 1.f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(1.f, 1.f, 1.f, 1.f);

	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.152f, 0.152f, 0.152f, 1.f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.152f, 0.152f, 0.152f, 1.f);

	style->Colors[ImGuiCol_Separator] = ImVec4(0.152f, 0.152f, 0.152f, 1.f);
	style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.152f, 0.152f, 0.152f, 1.f);

	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.211, 0.211, 0.211, 1.f);
	//style->WindowBorderSize = 0.0f;

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style->WindowRounding = 0.0f;
		style->Colors[ImGuiCol_WindowBg].w = 1.0f;
	}


	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	io.MouseDrawCursor = false;
	io.IniFilename = "imgui.ini";
	//io.IniFilename = NULL;

	return true;
}

bool M_Editor::Start()
{
	return true;
}

void M_Editor::Draw()
{

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	DrawMenuBar();

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0);
	DrawTopBar();
	ImGui::PopStyleVar();

	CreateDockSpace();

	//if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) 
	//{
	//	SaveStyle("Dark");
	//	//LoadStyle("Dark");
	//}


	//ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowMinSize, ImVec2(250, 250));
	for (unsigned int i = 0; i < windows.size(); i++)
	{
		if (windows[i]->active) 
		{
			//ASK: Docked windows need a min size ASAP
			//ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
			//ImGui::SetNextWindowSizeConstraints(ImVec2(300, 300), ImVec2(800, 800));
			windows[i]->Draw();
		}
	}
	//ImGui::PopStyleVar();

	if (displayWindow)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_MenuBarBg, ImVec4(0.f, 0.f, 0.f, 1.f));
		ImGui::ShowDemoWindow();
		ImGui::PopStyleColor();
	}

	//Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

}


bool M_Editor::CleanUp()
{
	LOG(LogType::L_NORMAL, "Editor CleanUp");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	for (unsigned int i = 0; i < windows.size(); ++i)
	{
		delete windows[i];
		windows[i] = nullptr;
	}
	windows.clear();

	LOG(LogType::L_NORMAL, "ImGui Shutdown");
	return true;
}

//Will draw the menu bar if the function call is placed 
//between NewFrame() and Render()
void M_Editor::DrawMenuBar()
{
	//UNITY: File edit assets gameobject component window help
	//Unreal Engine: File edit window help
	//Cry Engine: File edit modify display configspec group Prefabs Terrain Sound Game AI Clouds Tools View Help 
	//FrostByte: File edit scene Terrain Window
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.f, 0.f, 1.f));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			if (ImGui::MenuItem("New"))
			{
				//Do something
			}
			if (ImGui::MenuItem("Quit"))
			{
				App->ExitApplication();
			}
			ImGui::PopStyleColor(1);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			ImGui::MenuItem(windows[static_cast<int>(EditorWindow::CONFIGURATION)]->name.c_str(), nullptr, &windows[static_cast<int>(EditorWindow::CONFIGURATION)]->active);
			ImGui::PopStyleColor(1);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("GameObject"))
		{

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));

			//TODO: This is temporal, meshes should not laod every time and 
			//should be stored only once, then only copy id's.
			if (ImGui::MenuItem("Cube", nullptr))
				//MeshLoader::ImportFBX("Assets/cube.fbx", App->renderer3D->testMeshes, App->renderer3D->imgID);

			ImGui::MenuItem("Pyramid", nullptr);
			ImGui::MenuItem("Sphere", nullptr);
			ImGui::MenuItem("Cylinder", nullptr);
			if (ImGui::MenuItem("Monkey", nullptr))
				//MeshLoader::ImportFBX("Assets/monkey.fbx", App->renderer3D->testMeshes, App->renderer3D->imgID);

			ImGui::PopStyleColor();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			for (unsigned int i = 0; i < windows.size() - 2; i++)
			{
				ImGui::MenuItem(windows[i]->name.c_str(), nullptr, &windows[i]->active);
			}
			ImGui::PopStyleColor(1);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			
			//TODO: Put correct links on MenuItems
			if (ImGui::MenuItem("ImGui Demo", nullptr, displayWindow))
			{
				displayWindow = !displayWindow;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				ShellExecute(0, 0, "https://github.com/MayKoder/Diamond-Engine", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("Download latest"))
			{
				ShellExecute(0, 0, "https://github.com/MayKoder/Diamond-Engine/releases", 0, 0, SW_SHOW);
			}		
			if (ImGui::MenuItem("Report a bug"))
			{
				ShellExecute(0, 0, "https://github.com/MayKoder/Diamond-Engine/issues", 0, 0, SW_SHOW);
			}
			if (ImGui::MenuItem("MayKoder Github"))
			{
				ShellExecute(0, 0, "https://github.com/MayKoder", 0, 0, SW_SHOW);
			}
			ImGui::MenuItem(windows[static_cast<int>(EditorWindow::ABOUT)]->name.c_str(), nullptr, &windows[static_cast<int>(EditorWindow::ABOUT)]->active);

			ImGui::PopStyleColor(1);
			ImGui::EndMenu();
		}

		//Sets the play submenu in a dynamic dock position
		//ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetWindowSize().y));

		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleColor(1);
}

void M_Editor::DrawTopBar()
{
	//Main menu bar 2
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiContext& g = *GImGui;

	g.NextWindowData.MenuBarOffsetMinVal = ImVec2(g.Style.DisplaySafeAreaPadding.x, ImMax(g.Style.DisplaySafeAreaPadding.y - g.Style.FramePadding.y, 0.0f));

	ImGui::SetNextWindowSize(ImVec2(g.IO.DisplaySize.x, (g.NextWindowData.MenuBarOffsetMinVal.y + g.FontBaseSize + g.Style.FramePadding.y) * 2.5f));

	g.NextWindowData.MenuBarOffsetMinVal = ImVec2(0.f, 0.f);

	//TODO: This breaks JSON style loaging because it's forcing this color to be saved 
	//as the default background color :/
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.152f, 0.152f, 0.152f, 1.f));
	if (ImGui::Begin("ButtonsNavBar", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking)) 
	{

		viewportCorSize = ImGui::GetWindowSize().y;

		ImGuiStyle& style = ImGui::GetStyle();
		float w = ImGui::CalcItemWidth();
		float spacing = style.ItemInnerSpacing.x;
		float button_sz = ImGui::GetFrameHeight();
		ImGui::PushItemWidth((w - spacing * 2.0f - button_sz * 2.0f) * 0.13f);
		if (ImGui::BeginCombo("##styleDropdown", "Style"))
		{
			//It's never a good idea to change a vector content
			//while iterating over it
			bool fileChanged = false;
			for (int n = 0; n < styles.size(); n++)
			{
				if (ImGui::Button(styles[n].c_str()))
					ChangeStyleTo(styles[n].c_str());

				ImGui::SameLine();
				std::string label("X" + std::to_string(n));
				if (ImGui::Button(label.c_str()))
				{
					//Delete style
					DeleteStyle(styles[n].c_str());
					fileChanged = true;
				}
			}

			ImGui::Separator();

			//ERROR, WARNING, TODO: Temporal fix, this is bad, fix it
			ImGui::PushItemWidth(10 * MAX_STY_INPUT);
			ImGui::InputText("##sName: ", styleInput, MAX_STY_INPUT);
			ImGui::PopItemWidth();

			if (ImGui::Button("Save current style") && styleInput[0] != '\0')
			{
				SaveStyle(styleInput);
				styleInput[0] = '\0';
				fileChanged = true;
			}

			if (fileChanged)
				UpdateLoadedStylesVector(&styles);

			ImGui::EndCombo();
		}
		ImGui::PopItemWidth();

		//float halfWindowSize = ImGui::GetWindowSize().y - 15;

		//ImGui::SetCursorPosY((ImGui::GetWindowSize().y / 2) - (halfWindowSize / 2));

		//viewportCorSize = ImGui::GetWindowSize().y;

		//float initPoint = (ImGui::GetWindowSize().x * 0.5f) - ((halfWindowSize * 3.f) / 2.f);
		//ImGui::SetCursorPosX(initPoint + ((halfWindowSize + 10) * 0));
		//ImGui::Button("Pl", ImVec2(halfWindowSize, halfWindowSize));
		//ImGui::SameLine();

		//ImGui::SetCursorPosX(initPoint + ((halfWindowSize + 10) * 1));
		//ImGui::Button("Pa", ImVec2(halfWindowSize, halfWindowSize));
		//ImGui::SameLine();

		//ImGui::SetCursorPosX(initPoint + ((halfWindowSize + 10) * 2));
		//ImGui::Button("St", ImVec2(halfWindowSize, halfWindowSize));
	}
	ImGui::End();

	//ImGui::PopStyleColor();
}

void M_Editor::CreateDockSpace()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImVec2 dockPos(viewport->GetWorkPos());
	dockPos.y += viewportCorSize;
	ImGui::SetNextWindowPos(dockPos);

	ImVec2 dockSize(viewport->GetWorkSize());
	dockSize.y -= viewportCorSize;
	ImGui::SetNextWindowSize(dockSize);

	dockspace_id = ImGui::DockSpaceOverViewportCustom(viewport, ImGuiDockNodeFlags_PassthruCentralNode, dockPos, dockSize, nullptr);
}


//Returns nullptr if the window doesn't exist in the editor.
//Use dynamic_cast to convert from Window* to the type of editor window 
//pointer you want to use
Window* M_Editor::GetEditorWindow(EditorWindow type)
{
	unsigned int vecPosition = static_cast<unsigned int>(type);

	//SDL_assert(vecPosition < windows.size());
	return (vecPosition < windows.size()) ? windows[vecPosition] : nullptr;
}

void M_Editor::SaveStyle(const char* styleName)
{
	//Maybe learning json alone wasn't a good idea
	LOG(LogType::L_NORMAL, "Saving %s", styleName);

	JSON_Value* file = json_parse_file(STYLES_PATH);
	JSON_Object* root_object = json_value_get_object(file);

	//Init settings array
	JSON_Value* settingsArray = json_value_init_array();
	json_object_set_value(root_object, styleName, settingsArray);

	//Populate settings array
	JSON_Array* settings = json_value_get_array(settingsArray);
	ImGuiStyle& style = ImGui::GetStyle();

	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		JSON_Value *leaf_value = json_value_init_object();
		JSON_Object *leaf_object = json_value_get_object(leaf_value);

		json_object_set_number(leaf_object,"ID", i);
		json_object_set_number(leaf_object,"R", style.Colors[i].x);
		json_object_set_number(leaf_object,"G", style.Colors[i].y);
		json_object_set_number(leaf_object,"B", style.Colors[i].z);
		json_object_set_number(leaf_object,"A", style.Colors[i].w);

		json_array_append_value(settings, leaf_value);
	}

	//Save file 
	json_serialize_to_file_pretty(file, STYLES_PATH);

	//Free memory
	json_value_free(file);
}

void M_Editor::ChangeStyleTo(const char* styleName)
{
	JSON_Value* file = json_parse_file(STYLES_PATH);
	JSON_Object* root_object = json_value_get_object(file);

	JSON_Value* stArray = json_object_get_value(root_object, styleName);
	JSON_Array* settings = json_value_get_array(stArray);

	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4* mod = nullptr;
	for (int i = 0; i < ImGuiCol_COUNT; i++)
	{
		JSON_Object* a = json_array_get_object(settings, i);
		mod = &style.Colors[i];

		mod->x = json_object_get_number(a, "R");
		mod->y = json_object_get_number(a, "G");
		mod->z = json_object_get_number(a, "B");
		mod->w = json_object_get_number(a, "A");

	}

	json_value_free(file);
	//json_value_free(stArray);

	LOG(LogType::L_NORMAL, "Style %s loaded", styleName);
}

void M_Editor::DeleteStyle(const char* styleName)
{
	//Maybe learning json alone wasn't a good idea
	LOG(LogType::L_NORMAL, "Deleting style %s", styleName);

	//Init node
	JSON_Value* file = json_parse_file(STYLES_PATH);
	JSON_Object* root_object = json_value_get_object(file);

	json_object_remove(root_object, styleName);

	//json_object_set_value(root_object, styleName, NULL);

	//Save file 
	json_serialize_to_file_pretty(file, STYLES_PATH);

	//Free memory
	json_value_free(file);
}

void M_Editor::UpdateLoadedStylesVector(std::vector<std::string>* _styles)
{
	LOG(LogType::L_NORMAL, "Loading saved styles at default file");

	styles.clear();
	JSON_Value* file = json_parse_file(STYLES_PATH);

	if (file != NULL)
	{
		JSON_Object* root_object = json_value_get_object(file);
		size_t size = json_object_get_count(root_object);

		for (size_t i = 0; i < size; ++i)
		{
			_styles->push_back(json_object_get_name(root_object, i));
		}
	}
	else
	{
		LOG(LogType::L_WARNING, "Styles file could not be loaded, loading default style");
	}
	json_value_free(file);
}

void M_Editor::LogToConsole(const char* msg, LogType _type)
{
		W_Console* consoleWindow = dynamic_cast<W_Console*>(EngineExternal->moduleEditor->GetEditorWindow(EditorWindow::CONSOLE));

		if (consoleWindow != nullptr)
			consoleWindow->AddLog(msg, _type);
}
