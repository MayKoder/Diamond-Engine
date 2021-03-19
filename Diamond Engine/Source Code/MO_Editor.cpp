#ifndef STANDALONE

#include "Application.h"

#include "MMGui.h"
#include"IM_FileSystem.h"

//ImGui Includes
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl3.h"

#include"DETime.h"
#include"AssetDir.h"
#include"parson/parson.h"

#include "MO_Window.h"
#include "MO_Renderer3D.h"
#include "MO_Editor.h"
#include "MO_Scene.h"
#include "MO_ResourceManager.h"
#include"MO_Camera3D.h"
#include "MO_GUI.h"

//Window types
#include "WI_Configuration.h"
#include "WI_Console.h"
#include "WI_About.h"
#include "WI_Inspector.h"
#include "WI_Hierarchy.h"
#include "WI_Scene.h"
#include "WI_Assets.h"
#include "WI_Game.h"
#include "WI_TextEditor.h"

#include"GameObject.h"
#include"IM_TextureImporter.h"
#include"MO_Camera3D.h"
#include "MO_AudioManager.h"

//TODO: Do i really need all those includes?
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
	windows[static_cast<unsigned int>(EditorWindow::GAME)] = new W_Game();
	windows[static_cast<unsigned int>(EditorWindow::TEXTEDITOR)] = new W_TextEditor();

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

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style->WindowRounding = 0.0f;
		style->Colors[ImGuiCol_WindowBg].w = 1.0f;
	}


	ImGui_ImplSDL2_InitForOpenGL(App->moduleWindow->window, App->moduleRenderer3D->context);
	ImGui_ImplOpenGL3_Init();

	io.MouseDrawCursor = false;
	io.IniFilename = "Settings/imgui.ini";
	playingTint = ImVec4(1, 1, 1, 1);
	//io.IniFilename = NULL;

	return true;
}


bool M_Editor::Start()
{
	//W_TextEditor* txtEditor = dynamic_cast<W_TextEditor*>(GetEditorWindow(EditorWindow::TEXTEDITOR));
	//if(txtEditor != nullptr)
	//	txtEditor->SetTextFromFile("Assets/Scripts/Core.cs");

	return true;
}


void M_Editor::Draw()
{
	glClearColor(0.08f, 0.08f, 0.08f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->moduleWindow->window);
	ImGui::NewFrame();

	ImGuizmo::BeginFrame();

	DrawMenuBar();

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowBorderSize, 0);
	DrawTopBar();
	ImGui::PopStyleVar();

	CreateDockSpace();


	for (unsigned int i = 0; i < windows.size(); i++)
	{
		if (windows[i]->active)
		{
			windows[i]->Draw();
		}
	}

	if (ImGui::IsMouseReleased(ImGuiMouseButton_::ImGuiMouseButton_Left)) 
	{
		W_Hierarchy* hier = dynamic_cast<W_Hierarchy*>(GetEditorWindow(EditorWindow::HIERARCHY));
		hier->dropTarget = nullptr;
	}

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

#ifndef STANDALONE
	//Must manual cleanup to avoid leaks and crashed with the resource manager
	editorIcons.CleanUp(); 
#endif // !STANDALONE


	LOG(LogType::L_NORMAL, "ImGui Shutdown");
	return true;
}

//Draw menu bar
void M_Editor::DrawMenuBar()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.f, 0.f, 1.f));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			if (ImGui::MenuItem("New"))
			{
				//Do something
				App->moduleScene->CleanScene();
				App->moduleScene->current_scene[0] = '\0';
				App->moduleScene->current_scene_name[0] = '\0';
			}
			if (ImGui::MenuItem("Save scene", "CTRL+S"))
			{
				std::string sceneDir = M_FileSystem::OpenSaveAsDialog();
				App->moduleFileSystem->ToLocalAssetsPath(sceneDir);
				if (!sceneDir.empty())
				{
					App->moduleScene->SaveScene(sceneDir.c_str());
					App->moduleResources->NeedsDirsUpdate(App->moduleResources->assetsRoot);
				}

			}
			if (ImGui::MenuItem("Load scene"))
			{

				std::string sceneDir = M_FileSystem::OpenFileSelectDialog();
				
				//WARNINR: Can't we += .meta to the string?
				App->moduleFileSystem->ToLocalAssetsPath(sceneDir);
				if (!sceneDir.empty()) 
				{
					std::string metaDir = App->moduleResources->GetMetaPath(sceneDir.c_str());
					std::string test = App->moduleResources->LibraryFromMeta(metaDir.c_str());
					App->moduleScene->LoadScene(test.c_str());
				}
			}
			ImGui::GreySeparator();
			if (ImGui::MenuItem("Build Game"))
			{
				//std::string buildDir = M_FileSystem::OpenSaveAsDialog();
				//FileSystem::CreateDir("Standalone Build");
				
			}
			ImGui::GreySeparator();
			if (ImGui::MenuItem("Quit", "Esc"))
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
			DrawCreateMenu();
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


	if (ImGui::Begin("ButtonsNavBar", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDocking))
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

			//WARNING, TODO: Temporal fix, this is bad, fix it
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

		ImGui::SameLine((ImGui::GetContentRegionMax().x / 2.f) - 100);
		if (ImGui::BeginChild("##playBTS", ImVec2(130, ImGui::GetWindowContentRegionMax().y - style.FramePadding.y), false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoDecoration)) 
		{
			
			//Play game maybe if its clicked while game is playing, stop game?
			if (ImGui::ImageButton((ImTextureID)editorIcons.GetIconTextureID("PLAY"), ImVec2(17, 17), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), (DETime::state == GameState::PLAY) ? playingTint : ImVec4(0, 0, 0, 1)))
			{
				if (DETime::state == GameState::STOP) 
				{
					App->moduleScene->SaveScene("Library/Scenes/tmp.des");
					DETime::Play();
					EngineExternal->moduleAudio->StopAllSounds();
					EngineExternal->moduleAudio->PlayOnAwake();
				}
				else
				{
					DETime::Stop();
					EngineExternal->moduleAudio->StopAllSounds();
					EngineExternal->moduleAudio->UnLoadAllBanks();
					App->moduleScene->LoadScene("Library/Scenes/tmp.des");
					//App->moduleFileSystem->DeleteAssetFile("Library/Scenes/tmp.des"); //TODO: Duplicated code, mmove to method
				}
			}
			ImGui::SameLine();

			//Stop game if playing
			if (ImGui::ImageButton((ImTextureID)editorIcons.GetIconTextureID("STOP"), ImVec2(17, 17), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(0, 0, 0, 1)))
			{
				if (DETime::state == GameState::PLAY || DETime::state == GameState::PAUSE)
				{
					DETime::Stop();
					EngineExternal->moduleAudio->StopAllSounds();
					EngineExternal->moduleAudio->UnLoadAllBanks();

					char scene_name[64];
					strcpy(scene_name, App->moduleScene->current_scene_name);

					App->moduleScene->LoadScene("Library/Scenes/tmp.des");
					strcpy(App->moduleScene->current_scene_name, scene_name);

					//App->moduleFileSystem->DeleteAssetFile("Library/Scenes/tmp.des");
				}
			}
			ImGui::SameLine();

			//Step one frame forward
			if (ImGui::ImageButton((ImTextureID)editorIcons.GetIconTextureID("PAUSE"), ImVec2(17, 17), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(0, 0, 0, 1)))
			{
				DETime::Pause();
				if (DETime::state == GameState::PLAY)
				{
					EngineExternal->moduleAudio->ResumeAllSounds();
				}
				else if (DETime::state == GameState::PAUSE)
				{
					EngineExternal->moduleAudio->PauseAllSounds();
				}
			}

			ImGui::SameLine();
			//Step one frame forward
			if (ImGui::ImageButton((ImTextureID)editorIcons.GetIconTextureID("STEP"), ImVec2(17, 17), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), ImVec4(0, 0, 0, 1)))
				DETime::Step();
		}
		ImGui::EndChild();


		ImGui::SameLine(ImGui::GetContentRegionMax().x - (ImGui::GetButtonSize("Take Screenshoot").x + style.FramePadding.x));
		if (ImGui::Button("Take Screenshoot")) 
		{
			TextureImporter::TakeScreenshot(App->moduleCamera->editorCamera.resolvedFBO.GetFrameBuffer());
		}
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


void M_Editor::DrawCreateMenu()
{
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));

	if (ImGui::MenuItem("Empty", nullptr))
		App->moduleScene->CreateGameObject("Empty", App->moduleScene->root);

	if (ImGui::BeginMenu("3D"))
	{
		//TODO: This is temporal, meshes should not laod every time and 
		//should be stored only once, then only copy mesh pointers.
		if (ImGui::MenuItem("Cube", nullptr)) 
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Cube.fbx.meta").c_str());
		if (ImGui::MenuItem("Plane", nullptr))
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Plane.fbx.meta").c_str());
		if (ImGui::MenuItem("Cylinder", nullptr))
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Cylinder.fbx.meta").c_str());
		if (ImGui::MenuItem("Icosphere", nullptr))
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Icosphere.fbx.meta").c_str());
		if (ImGui::MenuItem("Pyramid", nullptr))
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Pyramid.fbx.meta").c_str());
		if (ImGui::MenuItem("Sphere", nullptr))
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Sphere.fbx.meta").c_str());
		if (ImGui::MenuItem("Torus", nullptr))
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Torus.fbx.meta").c_str());
		if (ImGui::MenuItem("Monkey", nullptr))
			EngineExternal->moduleScene->LoadModelTree(EngineExternal->moduleResources->LibraryFromMeta("Assets/Primitives/Monkey.fbx.meta").c_str());
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("UI"))
	{
		//TODO: This is temporal, meshes should not laod every time and 
		//should be stored only once, then only copy mesh pointers.
		if (ImGui::MenuItem("Canvas", nullptr))
			EngineExternal->moduleGui->CreateCanvas();

		if (ImGui::MenuItem("Image", nullptr))
			EngineExternal->moduleGui->CreateImage();

		if (ImGui::MenuItem("Button", nullptr))
			EngineExternal->moduleGui->CreateButton();

		if (ImGui::MenuItem("Checkbox", nullptr))
			EngineExternal->moduleGui->CreateCheckbox();

		if (ImGui::MenuItem("Text", nullptr))
			EngineExternal->moduleGui->CreateText();
		
		ImGui::EndMenu();
	}

	if (ImGui::MenuItem("Particle System", nullptr))
	{
		GameObject* auxObj= App->moduleScene->CreateGameObject("Particle System", App->moduleScene->root);
		auxObj->AddComponent(Component::TYPE::MATERIAL);
		auxObj->AddComponent(Component::TYPE::PARTICLE_SYSTEM);
	}

	if (ImGui::MenuItem("Game Camera", nullptr))
		App->moduleScene->CreateGameCamera("Game Camera");


	ImGui::PopStyleColor(1);
}

//Returns nullptr if the window doesn't exist in the editor.
//Use dynamic_cast to convert from Window* to the type of editor window 
//pointer you want to use
Window* M_Editor::GetEditorWindow(EditorWindow type)
{
	//TODO: Maybe use templates as enter and return types, like unity GetComponent system?
	unsigned int vecPosition = static_cast<unsigned int>(type);

	//SDL_assert(vecPosition < windows.size());
	return (vecPosition < windows.size()) ? windows[vecPosition] : nullptr;
}

//Save a style to JSON
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

//Change current style
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

/*Delete a style*/
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

/*Update saved styles*/
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

GameObject* M_Editor::GetSelectedGO()
{
	if (windows.size() == 0)
		return nullptr;

	W_Inspector* inspector =dynamic_cast<W_Inspector*>(GetEditorWindow(EditorWindow::INSPECTOR));
	return inspector->selectedGO;
}

GameObject* M_Editor::GetDraggingGO()
{
	W_Hierarchy* hier = dynamic_cast<W_Hierarchy*>(GetEditorWindow(EditorWindow::HIERARCHY));
	return hier->dropTarget;
}

void M_Editor::SetSelectedGO(GameObject* _obj)
{
	W_Inspector* inspector = dynamic_cast<W_Inspector*>(GetEditorWindow(EditorWindow::INSPECTOR));

	inspector->selectedGO = _obj;
	inspector->SetEditingResource(nullptr);
	//SetSelectedAsset(nullptr);
}

AssetDir* M_Editor::GetSelectedAsset()
{
	W_Assets* assets = dynamic_cast<W_Assets*>(GetEditorWindow(EditorWindow::ASSETS));
	return (assets == nullptr) ? nullptr: assets->selectedFile;
}

void M_Editor::SetSelectedAsset(AssetDir* _file)
{
	dynamic_cast<W_Assets*>(GetEditorWindow(EditorWindow::ASSETS))->selectedFile = _file;
}

void M_Editor::LogToConsole(const char* msg, LogType _type)
{
		W_Console* consoleWindow = dynamic_cast<W_Console*>(EngineExternal->moduleEditor->GetEditorWindow(EditorWindow::CONSOLE));

		if (consoleWindow != nullptr)
			consoleWindow->AddLog(msg, _type);
}

#endif