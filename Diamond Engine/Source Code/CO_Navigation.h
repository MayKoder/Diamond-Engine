#pragma once
#include "Component.h"
//#include "SDL/include/SDL_gamecontroller.h" ///ARNAU: PREGUNTAR A MAYK
#include <map>

enum class ACTIONSNAVIGATION {
	MOVE,
	EXECUTE
};

enum class BUTTONSANDJOYSTICKS {
	BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_BACK,
    BUTTON_GUIDE,
    BUTTON_START,
    BUTTON_LEFTTTRIGGER,
    BUTTON_RIGHTTRIGGERK,
    BUTTON_LEFTSHOULDER,
    BUTTON_RIGHTSHOULDER,
    BUTTON_DPAD_UP,
    BUTTON_DPAD_DOWN,
    BUTTON_DPAD_LEFT,
    BUTTON_DPAD_RIGHT,
    RIGHT_JOYSTICK_UP,
    RIGHT_JOYSTICK_DOWN,
    RIGHT_JOYSTICK_LEFT,
    RIGHT_JOYSTICK_RIGHT,
    LEFT_JOYSTICK_UP,
    LEFT_JOYSTICK_DOWN,
    LEFT_JOYSTICK_LEFT,
    LEFT_JOYSTICK_RIGHT,
    TOTAL_BUTTONS_AND_JOYSTICKS
};

struct ActionToRealize {
	ACTIONSNAVIGATION action;
	int uid_gameobject;
};

class C_Navigation :public Component {
public:
	C_Navigation(GameObject* gameObject);
	~C_Navigation() override;

	void Update() override;

    void Select();

#ifndef STANDALONE
    bool OnEditor() override;
#endif // !STANDALONE

private:
    std::map< int, ActionToRealize> map_of_buttons_and_joysticks;
    bool is_selected;
};
