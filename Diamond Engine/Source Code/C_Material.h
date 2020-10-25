#pragma once
#include "Component.h"


class C_Material : public Component
{
public:
	C_Material(GameObject* _gm);
	virtual ~C_Material();

	/*void Update() override;*/

	int textureID;
};