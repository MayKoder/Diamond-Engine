#include "Globals.h"
#include "Application.h"
#include "M_Editor.h"
#include "MaykMath.h"

//Window types
#include "W_Configuration.h"
#include "W_Console.h"


M_Editor::M_Editor(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	//reserve() does not work with [] operator
	windows = std::vector<Window*>(static_cast<unsigned int>(EditorWindow::MAX), nullptr);

	windows[static_cast<unsigned int>(EditorWindow::CONFIGURATION)] = new W_Configuration();
	windows[static_cast<unsigned int>(EditorWindow::CONSOLE)] = new W_Console();

}

M_Editor::~M_Editor()
{
}

bool M_Editor::Init()
{

	LOG("ImGui Editor Setup");

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
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.219f, 0.219f, 0.219f, 1.f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(1.f, 1.f, 1.f, 1.f);

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style->WindowRounding = 0.0f;
		style->Colors[ImGuiCol_WindowBg].w = 1.0f;
	}


	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	io.MouseDrawCursor = false;
	//io.IniFilename = "imgui.ini";
	io.IniFilename = NULL;

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

	//ImGui::ShowDemoWindow();

	//ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	//ImGui::SetNextWindowPos(ImVec2(main_viewport->GetWorkPos().x + 650, main_viewport->GetWorkPos().y + 20), ImGuiCond_FirstUseEver);
	//ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	for (unsigned int i = 0; i < windows.size(); i++)
	{
		if (windows[i]->active) {
			windows[i]->Draw();
		}
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
	LOG("Editor CleanUp");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	LOG("ImGui Shutdown");
	return true;
}

//Will draw the menu bar if the function call is placed 
//between NewFrame() and Render()
void M_Editor::DrawMenuBar()
{
	//UNITY: File edit assets gameobject component window help
	//Unreal Engine: File edit window help Aquest te una window amb tots els assets
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
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			//for (unsigned int i = 0; i < windows.size(); i++)
			//{
			//	ImGui::MenuItem(windows[i]->name.c_str(), nullptr, &windows[i]->active);
			//}
			ImGui::PopStyleColor(1);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
			if (ImGui::MenuItem("MayKoder Github"))
			{
				ShellExecute(0, 0, "https://github.com/MayKoder/Diamond-Engine", 0, 0, SW_SHOW);
			}
			ImGui::PopStyleColor(1);
			ImGui::EndMenu();
		}

		//Sets the play submenu in a dynamic dock position
		/*ImGui::SetNextWindowPos(ImVec2(0.0f, ImGui::GetWindowSize().y));*/

		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleColor(1);
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
