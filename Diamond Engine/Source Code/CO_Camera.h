#pragma once
#include "Component.h"

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

	void Update() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	float4x4 ViewMatrixOpenGL() const;
	float4x4 ProjectionMatrixOpenGL() const;

	void SetAspectRatio(float aspectRatio);

	void StartDraw();
	void EndDraw();

	void ReGenerateBuffer(int w, int h);

	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;

	Frustum camFrustrum;
	float fov;
	bool cullingState;
	bool MSAA;

	float2 texBufferSize;

//Movement logic
public: 
	void LookAt(const float3& Spot);

	//void Look(const float3 &Position, const float3&Reference, bool RotateAroundReference = false);
	void Move(const float3& Movement);
};