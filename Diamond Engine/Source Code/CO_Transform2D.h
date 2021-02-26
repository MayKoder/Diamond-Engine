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

	float4x4 GetGlobal2DTransform(int winWidth, int winHeight);

private:
	void UpdateTransform();
	void SetTransform(float locPosX, float locPosY, float locRotation);

public:
	float posX;
	float posY;
	float localPosX;
	float localPosY;
	
	float rotation;
	float localRotation;

	float sizeX;
	float sizeY;

private:
	bool updateTransform = false;
};

#endif // !__CO_TRANSFORM_2D_H__
