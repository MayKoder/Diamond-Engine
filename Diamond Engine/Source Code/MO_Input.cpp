#include "Globals.h"
#include "Application.h"
#include "MO_Input.h"
#include "ImGui/imgui_impl_sdl.h"

#include "MO_Renderer3D.h"
#include "MO_Window.h"
#include "IM_FileSystem.h"
#include "MO_Scene.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_BUTTONS);
	for (size_t i = 0; i < MAX_BUTTONS; ++i) {
		for (int j = 0; j < 2; ++j) {
			game_pad[i] = KEY_IDLE;
		}
	}
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG(LogType::L_NORMAL, "Init: SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG(LogType::L_ERROR, "SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	//Gamepad init
	SDL_Init(SDL_INIT_GAMECONTROLLER);

#ifdef STANDALONE
	SDL_SetRelativeMouseMode(SDL_TRUE);
#endif // STANDALONE


	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
		game_pad[i] = SDL_GameControllerGetButton(controller_player, (SDL_GameControllerButton)i);
	}

	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
		if (game_pad[i] == KEY_IDLE) {
			game_pad[i] == KEY_DOWN;
			break;
		}
		else {
			game_pad[i] == KEY_REPEAT;
			break;
		}

		if (game_pad[i] == KEY_REPEAT || game_pad[i] == KEY_DOWN) {
			game_pad[i] == KEY_UP;
			break;
		}
		else {
			game_pad[i] == KEY_IDLE;
			break;
		}		
	}

	while (SDL_PollEvent(&Events) == 1) {

		switch (Events.type) {
		case SDL_CONTROLLERDEVICEADDED: {
			int num_joystincks = SDL_NumJoysticks();
			for (int i = 0; i < num_joystincks; ++i) {
				if (i == 0) {
					if (SDL_GameControllerGetAttached(controller_player) == SDL_FALSE) {
						controller_player = SDL_GameControllerOpen(i);
						continue;
					}
				}
			}

			break; }
		case SDL_CONTROLLERDEVICEREMOVED:
			if (SDL_GameControllerGetAttached(controller_player) == SDL_FALSE) {
				SDL_GameControllerClose(controller_player);
				controller_player = nullptr;
			}
			break;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{

#ifndef STANDALONE
		ImGui_ImplSDL2_ProcessEvent(&e);
#endif // !STANDALONE

		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / SCREEN_SIZE;
			mouse_y = e.motion.y / SCREEN_SIZE;

			mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
			mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
			break;

			case (SDL_DROPFILE):
			{   
				//TODO: Change method name to duplicate on drop or something
				FileSystem::LoadDroppedFile(e.drop.file);
				SDL_free(e.drop.file);    // Free dropped_filedir memory
				break;
			}

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					App->moduleRenderer3D->OnResize(e.window.data1, e.window.data2);
				}

				if (e.window.event == SDL_WINDOWEVENT_CLOSE && e.window.windowID == SDL_GetWindowID(App->moduleWindow->window))
				{
					quit = true;
				}
			}
		}
	}

	if (keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		SDL_SetRelativeMouseMode(SDL_FALSE);

	//if (keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP || Events.type == SDL_QUIT)
	//	SDL_SetRelativeMouseMode(SDL_FALSE);

	if(quit == true)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG(LogType::L_NORMAL, "Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

#ifndef STANDALONE
void ModuleInput::OnGUI()
{
	if (ImGui::CollapsingHeader("Input", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Mouse: X = %d, Y = %d, Z = %d", mouse_x, mouse_y, mouse_z);
		ImGui::Text("Mouse motion: %d, %d", mouse_x_motion, mouse_y_motion);

		ImGui::Text("GamePad: Pressing A: %d", game_pad[SDL_CONTROLLER_BUTTON_A] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing B: %d", game_pad[SDL_CONTROLLER_BUTTON_B] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing X: %d", game_pad[SDL_CONTROLLER_BUTTON_X] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing Y: %d", game_pad[SDL_CONTROLLER_BUTTON_Y] == KEY_REPEAT);
	}
}
#endif // !STANDALONE
