#pragma once
#include "Component.h"
#include <map>

enum KEY_STATE;

enum class ACTIONSNAVIGATION {
    NONE,
	MOVE,
	EXECUTE,
    TOTAL_ACTIONS_WITH_NONE
};

enum class BUTTONSANDJOYSTICKS {
    NO_BUTTON_OR_JOYSTICK = -1,
	BUTTON_A,
    BUTTON_B,
    BUTTON_X,
    BUTTON_Y,
    BUTTON_BACK,
    BUTTON_GUIDE,
    BUTTON_START,
    BUTTON_LEFTTTRIGGER,
    BUTTON_RIGHTTRIGGER,
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
    ActionToRealize();
    ~ActionToRealize();


	ACTIONSNAVIGATION action;
	int uid_gameobject;
    bool is_key_down;
    bool is_key_up;
};

class C_Navigation :public Component {
public:
	C_Navigation(GameObject* gameObject, Component::TYPE type_of_ui);
	~C_Navigation() override;

	void Update() override;

    void CheckIfButtonOrJoystickIsBeingUsed(BUTTONSANDJOYSTICKS button_or_joystick_to_check, KEY_STATE& state);

    void DoTheAction(GameObject* gameobject, BUTTONSANDJOYSTICKS button_or_joystick_used, ACTIONSNAVIGATION action, bool is_key_released);

    void Select();
    void Deselect();

    void SaveData(JSON_Object* nObj) override;
    void LoadData(DEConfig& nObj) override;

    void SaveMapData(JSON_Object* nObj, ActionToRealize& action, BUTTONSANDJOYSTICKS map_index);
    void LoadMapaData(DEConfig& nObj);

#ifndef STANDALONE
    bool OnEditor() override;
    void WriteButtonOrJoystickOnEditor(const char* text,BUTTONSANDJOYSTICKS button_or_joystick);
#endif // !STANDALONE

private:
    std::map< BUTTONSANDJOYSTICKS, ActionToRealize> map_of_buttons_and_joysticks;
    bool is_selected;
    BUTTONSANDJOYSTICKS button_or_joystick_being_used;
    Component::TYPE type_of_ui;
};
