#include "CO_Canvas.h"

C_Canvas::C_Canvas(GameObject* gameObject) : Component(gameObject)
{
}


C_Canvas::~C_Canvas()
{
}


#ifndef STANDALONE
bool C_Canvas::OnEditor()
{
	//Draw vertex info and canvas options
	return true;
}
#endif // !STANDALONE