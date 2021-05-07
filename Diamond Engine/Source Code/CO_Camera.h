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

	static inline Type GetType() { return Type::Camera; }; //This will allow us to get the type from a template

	void Update() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	float4x4 ViewMatrixOpenGL() const;
	float4x4 ProjectionMatrixOpenGL() const;

	void SetAspectRatio(float aspectRatio);

	void StartDraw();
	void EndDraw();

	void ReGenerateBuffer(int w, int h);

	void PushCameraMatrix();

	bool IsInsideFrustum(AABB& globalAABB);

	DE_FrameBuffer resolvedFBO;
	DE_FrameBuffer msaaFBO;

	Frustum camFrustrum;
	float fov;
	float orthoSize;
	bool cullingState;

private:
	bool OrthoCulling(AABB& globalAABB);
	bool PrespectiveCulling(AABB& globalAABB);

//Movement logic
public: 
	void LookAt(const float3& Spot);
	static void LookAt(Frustum& frust, const float3& Spot);

	void Move(const float3& Movement);
	float3 GetPosition();

private:
	int msaaSamples;
};