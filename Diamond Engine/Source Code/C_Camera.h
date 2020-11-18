#pragma once
#include "Component.h"

#include"MathGeoLib/include/Geometry/Frustum.h"

class C_Camera : public Component
{
public:
	C_Camera(GameObject* _gm);
	virtual ~C_Camera();

	/*void Update() override;*/
	bool OnEditor() override;

	void Update() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(JSON_Object* nObj) override;

	Frustum camFrustrum;
	int imgBufferID;

};