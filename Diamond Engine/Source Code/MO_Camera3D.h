#pragma once
#include "Module.h"

#include"CO_Camera.h"
#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/Quat.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	virtual ~ModuleCamera3D();

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

#ifndef STANDALONE
	void OnGUI() override;
#endif // !STANDALONE

	void ProcessSceneKeyboard();

private:

	void OrbitalRotation(float3 center, float dt);
	void FreeRotation(float dt);
	void FocusCamera(float3 center, float offset);
	void PanCamera(float);

public:
	
	//float3 X, Y, Z, Reference;

	float mouseSensitivity;
	float cameraSpeed;

	C_Camera editorCamera;

private:
	float3 cameraMovement;
};