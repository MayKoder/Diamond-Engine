#pragma once
#include "Component.h"
#include"DE_FrameBuffer.h"

#include"MathGeoLib/include/Geometry/Frustum.h"

class C_Camera : public Component
{
public:
	C_Camera();
	C_Camera(GameObject* _gm);
	virtual ~C_Camera();

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void PostUpdate() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	float4x4 ViewMatrixOpenGL() const;
	float4x4 ProjectionMatrixOpenGL() const;

	void SetAspectRatio(float aspectRatio);

	void StartDraw();
	void EndDraw();

	void ReGenerateBuffer(int w, int h);

	void PushCameraMatrix();

	DE_FrameBuffer resolvedFBO;
	DE_FrameBuffer msaaFBO;

	Frustum camFrustrum;
	float fov;
	float orthoSize;
	bool cullingState;

	float windowWidth;
	float windowHeight;

//Movement logic
public: 
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	float3 GetPosition();

private:
	int msaaSamples;
};