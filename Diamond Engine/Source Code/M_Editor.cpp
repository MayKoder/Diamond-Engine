#include "Globals.h"
#include "Application.h"
#include "M_Editor.h"
#include "MaykMath.h"


M_Editor::M_Editor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
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
	ImGuiStyle& style2 = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style2.WindowRounding = 0.0f;
		style2.Colors[ImGuiCol_WindowBg].w = 1.0f;
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

	ImGui::Begin("Configuration", NULL);
	ImGui::BulletText("ImGui  v%s", ImGui::GetVersion());



	ImGui::End();

	ImGui::Begin("Configuration 2", NULL);
	ImGui::BulletText("ImGui  v%s", ImGui::GetVersion());



	ImGui::End();

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
				ShellExecute(0, 0, "https://github.com/MayKoder", 0, 0, SW_SHOW);
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

