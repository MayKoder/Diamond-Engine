#include "Globals.h"
#include "Application.h"
#include "MO_Input.h"
#include "ImGui/imgui_impl_sdl.h"

#include "MO_Renderer3D.h"
#include "MO_Window.h"
#include "IM_FileSystem.h"
#include "MO_Scene.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled), haptic(nullptr), hapticEnabled(true)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);
	for (size_t i = 0; i < MAX_MOUSE_BUTTONS; ++i) {
		for (int j = 0; j < 2; ++j) {
			game_pad[i] = KEY_IDLE;
		}
	}

	for (size_t i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++)
	{
		game_pad[i] = KEY_IDLE;
	}
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
	keyboard = nullptr;
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

	//Haptic init
	SDL_Init(SDL_INIT_HAPTIC);

	//Open Joystick for haptic usage
	if (SDL_NumJoysticks() > 0) {
		SDL_Joystick* joystick = SDL_JoystickOpen(0);
		if (joystick)
			LOG(LogType::L_NORMAL, "Opened Joystick 0");

		//Check if is haptic
		if (SDL_JoystickIsHaptic(joystick) == 1) {
			LOG(LogType::L_NORMAL, "Is Haptic");
		}

		//Open the device
		haptic = SDL_HapticOpenFromJoystick(joystick);
		if (haptic == nullptr) return ret;

		if (SDL_HapticRumbleInit(haptic) == 0) {
			LOG(LogType::L_NORMAL, "Rumlbe Init Innit");
		}

	}


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
		bool key_pressed = SDL_GameControllerGetButton(controller_player, (SDL_GameControllerButton)i);

		if (key_pressed)
		{
			switch (game_pad[i])
			{
			case KEY_IDLE: 
				game_pad[i] = KEY_DOWN;
				break;
			case KEY_DOWN:
				game_pad[i] = KEY_REPEAT; 
				break;
			case KEY_UP: 
				game_pad[i] = KEY_DOWN; 
				break;
			default:
				break;
			}
		}
		else
		{
			switch (game_pad[i])
			{
				case KEY_DOWN: 
					game_pad[i] = KEY_UP; 
					break;
				case KEY_REPEAT:
					game_pad[i] = KEY_UP;
					break;
				case KEY_UP:
					game_pad[i] = KEY_IDLE;
					break;
				default: 
					break;
			}
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

	if (keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		SDL_SetRelativeMouseMode(SDL_FALSE);

	if(quit == true)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG(LogType::L_NORMAL, "Quitting SDL input event subsystem");
	SDL_GameControllerClose(controller_player);
	SDL_HapticClose(haptic);
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
	SDL_QuitSubSystem(SDL_INIT_HAPTIC);
	return true;
}

#ifndef STANDALONE
void ModuleInput::OnGUI()
{
	if (ImGui::CollapsingHeader("Input", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("----------Mouse Inputs----------");
		ImGui::Text("Mouse: X = %d, Y = %d, Z = %d", mouse_x, mouse_y, mouse_z);
		ImGui::Text("Mouse motion: %d, %d", mouse_x_motion, mouse_y_motion);
		
		ImGui::Text("----------GamePad Inputs----------");
		ImGui::Text("-----BUTTONS-----");
		ImGui::Text("GamePad: Pressing A: %d", game_pad[SDL_CONTROLLER_BUTTON_A] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing B: %d", game_pad[SDL_CONTROLLER_BUTTON_B] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing X: %d", game_pad[SDL_CONTROLLER_BUTTON_X] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing Y: %d", game_pad[SDL_CONTROLLER_BUTTON_Y] == KEY_REPEAT);

		ImGui::Text("-----AXIS PRESSING-----");
		ImGui::Text("GamePad: Pressing Left Stick: %d", game_pad[SDL_CONTROLLER_BUTTON_LEFTSTICK] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing Right Stick: %d", game_pad[SDL_CONTROLLER_BUTTON_RIGHTSTICK] == KEY_REPEAT);

		ImGui::Text("-----AXIS MOVEMENT-----");
		ImGui::Text("GamePad: Moving Left Stick Left/Right: %d", SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_LEFTX));
		ImGui::Text("GamePad: Moving Left Stick Up/Down: %d", SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_LEFTY));	
		ImGui::Text("GamePad: Moving Right Stick Left/Right: %d", SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_RIGHTX));
		ImGui::Text("GamePad: Moving Right Stick Up/Down: %d", SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_RIGHTY));
		
		ImGui::Text("-----SHOULDERS-----");
		ImGui::Text("GamePad: Pressing Left Shoulder: %d", game_pad[SDL_CONTROLLER_BUTTON_LEFTSHOULDER] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing Right Shoulder: %d", game_pad[SDL_CONTROLLER_BUTTON_RIGHTSHOULDER] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing LT: %d", SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
		ImGui::Text("GamePad: Pressing RT: %d", SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));

		ImGui::Text("-----MIDDLE BUTTONS-----");
		ImGui::Text("GamePad: Pressing Start: %d", game_pad[SDL_CONTROLLER_BUTTON_START] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing Back: %d", game_pad[SDL_CONTROLLER_BUTTON_BACK] == KEY_REPEAT);

		ImGui::Text("-----DPad Buttons-----");
		ImGui::Text("GamePad: Pressing DPad Up: %d", game_pad[SDL_CONTROLLER_BUTTON_DPAD_UP] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing DPad Down: %d", game_pad[SDL_CONTROLLER_BUTTON_DPAD_DOWN] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing DPad Left: %d", game_pad[SDL_CONTROLLER_BUTTON_DPAD_LEFT] == KEY_REPEAT);
		ImGui::Text("GamePad: Pressing DPad Right: %d", game_pad[SDL_CONTROLLER_BUTTON_DPAD_RIGHT] == KEY_REPEAT);
	}
}
#endif // !STANDALONE
void ModuleInput::PlayHaptic(float strength, int length)
{
	if (hapticEnabled)
		SDL_HapticRumblePlay(haptic, strength, length);
}