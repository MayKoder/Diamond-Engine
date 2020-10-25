#ifndef __W_INSPECTOR_H__
#define __W_INSPECTOR_H__

#include "Window.h"

class GameObject;

class W_Inspector : public Window
{

public:
	W_Inspector();
	virtual ~W_Inspector();

	void Draw() override;

	GameObject* selectedGO;


};

#endif //__W_INSPECTOR_H__

