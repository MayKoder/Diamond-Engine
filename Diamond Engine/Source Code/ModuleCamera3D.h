#pragma once
#include "Module.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void OnGUI() override;

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

private:

	void CalculateViewMatrix();

	void OrbitalRotation(vec3 center, float dt);
	void FreeRotation(float dt);
	void FocusCamera(vec3 center, float offset);
	void PanCamera(float);

public:
	
	vec3 X, Y, Z, Position, Reference;

	float mouseSensitivity;
	float cameraSpeed;


private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
	mat4x4 followPoint;

	vec3 cameraMovement;

};