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


float GetTextColor(MonoObject* obj, int index) {
	if (EngineExternal == nullptr)
		return 0;

	C_Text* workText = DECS_CompToComp<C_Text*>(obj);

	if (workText == nullptr)
		return 0;

	return workText->GetColorText(index);
}

void SetTextColor(MonoObject* obj, float value, int index) {
	if (EngineExternal == nullptr)
		return;

	C_Text* workText = DECS_CompToComp<C_Text*>(obj);

	if (workText == nullptr)
		return;

	workText->SetColorText(index, value);
}


float GetTextRed(MonoObject* obj)
{
	return GetTextColor(obj, 0);
}

void SetTextRed(MonoObject* obj, float value)
{
	SetTextColor(obj, value, 0);
}

float GetTextGreen(MonoObject* obj)
{
	return GetTextColor(obj, 1);
}

void SetTextGreen(MonoObject* obj, float value)
{
	SetTextColor(obj, value, 1);
}

float GetTextBlue(MonoObject* obj)
{
	return GetTextColor(obj, 2);
}

void SetTextBlue(MonoObject* obj, float value)
{
	SetTextColor(obj, value, 2);
}