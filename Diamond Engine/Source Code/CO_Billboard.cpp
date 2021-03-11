#include "CO_Billboard.h"

C_Billboard::C_Billboard(GameObject* _gm):Component(_gm), currentAlignment(BILLBOARDALIGNMENT::SCREEN_ALIGNED)
{
}

C_Billboard::~C_Billboard()
{
}

#ifndef STANDALONE
bool C_Billboard::OnEditor()
{
	return false;
}
#endif // !STANDALONE

void C_Billboard::SetAlignment(BILLBOARDALIGNMENT new_alignment)
{
}

void C_Billboard::ScreenAlign()
{
}

void C_Billboard::WorldAlign()
{
}

void C_Billboard::AxisAlign()
{
}
