#ifndef STANDALONE

#ifndef __W_HIERARCHY_H__
#define __W_HIERARCHY_H__

#include "Window.h"

class M_Scene;
class GameObject;

class W_Hierarchy : public Window
{

public:
	W_Hierarchy(M_Scene* _scene);
	virtual ~W_Hierarchy();

	void Draw() override;

	void SetCurrentScene(M_Scene* _scene);

	GameObject* dropTarget;
private:
	//Maybe use stack iteration and not recursive iteration
	void DrawGameObjectsTree(GameObject* node, bool drawAsDisabled);

	M_Scene* cSceneReference;
};

#endif //__W_HIERARCHY_H__

#endif // !STANDALONE