#ifndef __W_INSPECTOR_H__
#define __W_INSPECTOR_H__

#include "Window.h"

class GameObject;

#define MAX_NAME_SIZE 50
class W_Inspector : public Window
{

public:
	W_Inspector();
	virtual ~W_Inspector();

	void Draw() override;

	GameObject* selectedGO;
	char inputName[MAX_NAME_SIZE];

};

#endif //__W_INSPECTOR_H__

