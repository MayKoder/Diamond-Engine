#include "CO_Canvas.h"

#include "GameObject.h"

#include "Application.h"
#include "MO_GUI.h"

C_Canvas::C_Canvas(GameObject* gameObject) : Component(gameObject)
{
	EngineExternal->moduleGui->SetCanvas(gameObject->UID);
}

C_Canvas::~C_Canvas()
{
	EngineExternal->moduleGui->EraseCanvas();
}

#ifndef STANDALONE
bool C_Canvas::OnEditor()
{
	//Draw vertex info and canvas options
	return true;
}
#endif // !STANDALONE