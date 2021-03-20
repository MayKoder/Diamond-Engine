#pragma once
#include "Module.h"
#include"SDL/include/SDL_gamecontroller.h"
#include "SDL/include/SDL_haptic.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, bool start_enabled = true);
	virtual ~ModuleInput();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;

#ifndef STANDALONE
	void OnGUI() override;
#endif // !STANDALONE

	//Keyboard functions
	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}

	//Game Pad functions
	//Returns KEY_STATE of gamePad buttons: KEY_IDLE, KEY_DOWN, KEY_REPEAT, KEY_UP
	KEY_STATE GetGamePadKey(int id) const {
		return game_pad[id];
	}

	//Return values between -32768 (joystick up) and 32767 (joystick down). Value by default is -1 (idle)
	int GetLeftAxisY() {
		return SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_LEFTY);
	}

	//Return values between -32768 (joystick left) and 32767 (joystick right). Value by default is 0 (idle)
	int GetLeftAxisX (){
		return SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_LEFTX);
	}
	
	//Return values between -32768 (joystick up) and 32767 (joystick down). Value by default is -1 (idle)
	int GetRightAxisY() {
		return SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_RIGHTY);
	}

	//Return values between -32768 (joystick left) and 32767 (joystick right). Value by default is 0 (idle)
	int GetRightAxisX() {
		return SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_RIGHTX);
	}

	//Return values between 0 (trigger idle) and 32767 (trigger pressed)
	int GetRightTrigger() {
		return SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	}

	//Return values between 0 (trigger idle) and 32767 (trigger pressed)
	int GetLeftTrigger() {
		return SDL_GameControllerGetAxis(controller_player, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
	}

	//Execute haptic
	void PlayHaptic(float strength, int length);

public:
	bool hapticEnabled;

private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];

	//Gamepad control
	KEY_STATE game_pad[SDL_CONTROLLER_BUTTON_MAX];
	SDL_GameController* controller_player = nullptr;

	//Haptic
	SDL_Haptic* haptic;

	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;
	//int mouse_z_motion;
};