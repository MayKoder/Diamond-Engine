#pragma once

#include "CO_Navigation.h"

void SelectNavigation(MonoObject* obj)
{
	if (EngineExternal == nullptr)
		return;




	C_Navigation* workNav = DECS_CompToComp<C_Navigation*>(obj);

	if (workNav == nullptr)
		return;

	
	workNav->Select();
}