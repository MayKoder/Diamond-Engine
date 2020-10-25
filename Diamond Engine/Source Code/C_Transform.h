#pragma once
#include "Component.h"
#include"MathGeoLib/include/Math/float4x4.h"


class C_Transform : public Component
{
public:
	C_Transform(GameObject* _gm);
	virtual ~C_Transform();

	void Update() override;

	void OnEditor() override;

	float4x4 globalTransform;
	float4x4 localTransform;

};