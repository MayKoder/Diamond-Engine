#pragma once

#include "CO_Material.h"
#include "RE_Material.h"

void SetFloatUniform(MonoObject* obj, MonoString* name_uniform, float value)
{
	if (EngineExternal == nullptr)
		return;

	


	C_Material* workMat = DECS_CompToComp<C_Material*>(obj);

	if (workMat == nullptr)
		return;
	char* text = mono_string_to_utf8(name_uniform);
	int i;
	bool uniform_found = false;
	for (i = 0; i < workMat->material->uniforms.size();i++) {
		if (strcmp(workMat->material->uniforms[i].name, text) == 0) {
			uniform_found = true;
			break;
		}
	}
	if (!uniform_found)
		return;
	workMat->material->uniforms[i].data.floatValue = value;
	workMat->material->PushUniforms();

}

void SetIntUniform(MonoObject* obj, MonoString* name_uniform, int value)
{
	if (EngineExternal == nullptr)
		return;




	C_Material* workMat = DECS_CompToComp<C_Material*>(obj);

	if (workMat == nullptr)
		return;
	char* text = mono_string_to_utf8(name_uniform);
	int i;
	bool uniform_found = false;
	for (i = 0; i < workMat->material->uniforms.size(); i++) {
		if (strcmp(workMat->material->uniforms[i].name, text) == 0) {
			uniform_found = true;
			break;
		}
	}
	if (!uniform_found)
		return;
	workMat->material->uniforms[i].data.intValue = value;
	workMat->material->PushUniforms();

}