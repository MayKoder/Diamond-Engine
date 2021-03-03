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

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	void SetTransform(float locPosX, float locPosY, float locRotation, float sizeX, float sizeY);

	void SetTrueUpdateTransform();

private:
	void UpdateTransform();
	void SetPreviousParameters();

public:
	float position[2];
	float localPos[2];
	
	float rotation;
	float localRotation;

	float size[2];


private:
	bool updateTransform, send_command;
	float previous_parameters[5];////[0]&&[1] = position; [2] = rotation; [3] && [4] = size
};

#endif // !__CO_TRANSFORM_2D_H__
