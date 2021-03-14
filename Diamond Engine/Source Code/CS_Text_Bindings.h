#pragma once

#include "CO_Text.h"
MonoString* GetText(MonoObject* obj)
{
	if (EngineExternal == nullptr)
		return nullptr;

	C_Text* workText = DECS_CompToComp<C_Text*>(obj);

	if (workText == nullptr)
		return nullptr;

	const char* text = workText->GetText();

	return mono_string_new(EngineExternal->moduleMono->jitDomain, text);


}
void SetText(MonoObject* obj, MonoString* secObj) 
{
	if (EngineExternal == nullptr)
		return;

	char* text = mono_string_to_utf8(static_cast<MonoString*>(secObj));
	

	C_Text* workText = DECS_CompToComp<C_Text*>(obj);

	if (workText == nullptr)
		return;

	if (workText)
		workText->SetText(text);

	mono_free(text);
}