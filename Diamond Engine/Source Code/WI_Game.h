#ifndef STANDALONE

#ifndef __W_GAME_H__
#define __W_GAME_H__

#include "Window.h"

class C_Camera;

class W_Game : public Window
{

public:
	W_Game();
	virtual ~W_Game();

	void Draw() override;

	void SetTargetCamera(C_Camera* _cam);
	C_Camera* GetTargetCamera();

private: 
	C_Camera* targetCamera;
	float aspect[2];

};

#endif //__W_GAME_H__

#endif // !STANDALONE