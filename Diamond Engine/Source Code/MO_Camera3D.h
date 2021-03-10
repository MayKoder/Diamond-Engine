#pragma once
#include "Module.h"

#include"CO_Camera.h"
#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/Quat.h"
#ifndef STANDALONE

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	virtual ~ModuleCamera3D();

	bool Start() override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void OnGUI() override;

	void ProcessSceneKeyboard();

private:

	//moves the camera and the reference a certain amount
	void Move(const float3& movement);
	//moves the reference to the desired destination and the camera to a relative postion to the reference
	void MoveTo(const float3& destination);
	//orbits the camera around the reference 
	void OrbitalRotation(float dt);
	//Rotates the camera as in a FPS
	void FreeRotation(float dt);

	//void FocusCamera(float3 center, float offset); //Deprecated

	void PanCamera(float);
	//increments the camera zoom level, note that zoom level refers to the distance from the camera to the reference point in levels
	void CamZoom(int addZoomAmount);

public:
	
	//float3 X, Y, Z, Reference;

	//used in cam rotation
	float mouseSensitivity; 
	//used in cam movement
	float cameraSpeed;

	C_Camera editorCamera;

private:
	float3 cameraMovement;
	float3 reference;
	float zoomLevel;
};
#endif // !STANDALONE