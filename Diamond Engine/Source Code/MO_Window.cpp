#include "Globals.h"
#include "Application.h"
#include "MO_Window.h"
#include "EngineVersion.h"
#include "ImGui/imgui.h"
#include "MO_Renderer3D.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled), s_width(0), s_height(0), brightness(1.f)
, fullScreen(false), borderless(false), resizable(true), fullScreenDesktop(false)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG(LogType::L_NORMAL, "Init: SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG(LogType::L_ERROR, "SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		s_width = SCREEN_WIDTH * SCREEN_SIZE;
		s_height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//SDL_GL Attibutes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(fullScreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullScreenDesktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, s_width, s_height, flags);

		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		auto sWidth = DM.w;
		auto sHeight = DM.h;

		SDL_SetWindowSize(window, MIN_WIDTH, MIN_HEIGHT);
		SDL_SetWindowPosition(window, (sWidth / 2) - (MIN_WIDTH / 2), (sHeight / 2) - (MIN_HEIGHT / 2));

		SDL_SetWindowMinimumSize(window, MIN_WIDTH, MIN_HEIGHT);
		SDL_MaximizeWindow(window);

		SDL_GetWindowSize(window, &s_width, &s_height);

		if(window == NULL)
		{
			LOG(LogType::L_ERROR, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	//Title Init
	const std::string title(TITLE + EngineVersion::GetVersion() + " by Miquel Suau Gonzalez (MayKoder)");
	SetTitle(title.c_str());

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG(LogType::L_NORMAL, "Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

#ifndef STANDALONE
void ModuleWindow::OnGUI()
{
	if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_DefaultOpen))
	{
		SDL_DisplayMode current;
		int i = 0;
		SDL_GetCurrentDisplayMode(i, &current);

		if (ImGui::SliderFloat("Brightness", &brightness, 0.f, 1.f))
		{
			SDL_SetWindowBrightness(window, brightness);
		}

		ImGui::Text("Window width: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d", s_width);
		ImGui::Text("Window height: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d", s_height);

		ImGui::Text("Refresh rate: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%d", current.refresh_rate);

		if (ImGui::Checkbox("Fullscreen", &fullScreen)) {
			(fullScreen == true) ? SDL_SetWindowFullscreen(App->moduleWindow->window, SDL_WINDOW_FULLSCREEN): SDL_SetWindowFullscreen(App->moduleWindow->window, 0);
			
			SDL_GetWindowSize(window, &s_width, &s_height);
			App->moduleRenderer3D->OnResize(s_width, s_height);
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change fullscreen mode");

		ImGui::SameLine();

		if (ImGui::Checkbox("Resizable", &resizable))
			SDL_SetWindowResizable(App->moduleWindow->window, static_cast<SDL_bool>(resizable));
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change resizable mode");

		if (ImGui::Checkbox("Borderless", &borderless)) {
			SDL_SetWindowBordered(App->moduleWindow->window, static_cast<SDL_bool>(!borderless));
			SDL_GetWindowSize(window, &s_width, &s_height);
			App->moduleRenderer3D->OnResize(s_width, s_height);
		}

		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change borderless mode");

		ImGui::SameLine();

		if (ImGui::Checkbox("Fullscreen Desktop", &fullScreenDesktop)) {
			(fullScreenDesktop) ? SDL_SetWindowFullscreen(App->moduleWindow->window, SDL_WINDOW_FULLSCREEN_DESKTOP) : SDL_SetWindowFullscreen(App->moduleWindow->window, 0);
			SDL_GetWindowSize(window, &s_width, &s_height);
			App->moduleRenderer3D->OnResize(s_width, s_height);
		}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Change fullscreen desktop mode");

		ImGui::NewLine();
	}
}
#endif // !STANDALONE

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}