#ifndef __CO_TRANSFORM_2D_H__
#define __CO_TRANSFORM_2D_H__

#include "Component.h"

class C_Transform2D : public Component
{
public:
	C_Transform2D(GameObject* gameObject);
	~C_Transform2D() override;

	void Update() override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	float4x4 GetGlobal2DTransform();

private:
	void UpdateTransform();
	void SetTransform(float locPosX, float locPosY, float locRotation);

public:
	float position[2];
	float localPos[2];
	
	float rotation;
	float localRotation;

	float size[2];

private:
	bool updateTransform = false;
};

#endif // !__CO_TRANSFORM_2D_H__
