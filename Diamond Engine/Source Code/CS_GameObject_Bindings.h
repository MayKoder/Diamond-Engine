#pragma once

#include "Application.h"
#include "MO_MonoManager.h"
#include "GameObject.h"
#include "CS_Transform_Bindings.h"

MonoString* GetTag(MonoObject* cs_Object)
{
	GameObject* cpp_gameObject = DECS_Comp_To_GameObject(cs_Object);

	return mono_string_new(EngineExternal->moduleMono->domain, cpp_gameObject->tag);
}