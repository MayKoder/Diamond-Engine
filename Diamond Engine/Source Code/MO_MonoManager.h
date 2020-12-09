#pragma once
#include "Module.h"

#include <mono/utils/mono-publib.h>
typedef struct _MonoDomain MonoDomain;
typedef struct _MonoMethod MonoMethod;
typedef struct _MonoObject MONO_RT_MANAGED_ATTR MonoObject;

class M_MonoManager : public Module
{
public:
	M_MonoManager(Application* app, bool start_enabled = true);
	virtual ~M_MonoManager();

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

public:
	MonoDomain* domain;
	MonoObject* object;
	MonoMethod* method;
};