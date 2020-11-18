#ifndef __W_GAME_H__
#define __W_GAME_H__

#include "Window.h"

class W_Game : public Window
{

public:
	W_Game();
	virtual ~W_Game();

	void Draw() override;

};

#endif //__W_GAME_H__

