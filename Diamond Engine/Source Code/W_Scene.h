#ifndef __W_SCENE_H__
#define __W_SCENE_H__

#include "Window.h"
#include "OpenGL.h"
#include"Application.h"

class W_Scene : public Window
{

public:
	W_Scene(Application* _app);
	virtual ~W_Scene();

	void InitSceneView();

	void Draw() override;

	Application* App;

};

#endif //__W_SCENE_H__

