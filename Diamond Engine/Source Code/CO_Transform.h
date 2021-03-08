#pragma once
#include "Component.h"

#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/Quat.h"

#include<vector>

class GameObject;

class C_Transform : public Component
{
public:

	C_Transform();
	C_Transform(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/);
	virtual ~C_Transform();

	void Update() override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	static inline Type GetType() { return Type::Transform; }; //This will allow us to get the type from a template

	float4x4 globalTransform;
	float4x4 localTransform;

	float3 position, localScale, eulerRotation;
	Quat rotation;

	bool updateTransform;
	void SetTransformMatrix(float3 _position, Quat _rotation, float3 _localScale);

	void SetTransformWithGlobal(float4x4& globalMat);

	const float* GetGlobalTransposed() const;
	void ResetTransform();

	float3 GetForward();
	float3 GetUp();
	float3 GetRight();
	void UpdateBoxes();

private: 
	void UpdateTransform();
	C_Transform* GetRecursiveTransforms(C_Transform* node, std::vector<C_Transform*>& transforms);


	float4x4 globalTransformTRANS;

	float3 GetNormalizeAxis(int i);
};