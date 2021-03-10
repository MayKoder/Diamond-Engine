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
	bool updateTransform = false;
	bool send_command = false;
	bool maintainSize = false;
	float previous_transform[5];////[0]&&[1] = position; [2] = rotation; [3] && [4] = size

	float pixelSize[2];	//Used to maintain the size of the image when resizing the window
};

#endif // !__CO_TRANSFORM_2D_H__
