#include "CO_Collider.h"

#include "Globals.h"
#include <vector>
#include"GameObject.h"


C_Collider::C_Collider() : Component(nullptr)
{


	name = "Rigidbody";
}


C_Collider::C_Collider(GameObject* _gm/*, float3 _position, Quat _rotation, float3 _localScale*/) : Component(_gm)/*,
position(_position), rotation(_rotation), localScale(_localScale)*/
{


	name = "Transform";
}

C_Collider::~C_Collider()
{
}

void C_Collider::Update()
{
	/*if (updateTransform)
		UpdateTransform();*/
}

#ifndef STANDALONE
bool C_Collider::OnEditor()
{
	if (Component::OnEditor() == true)
	{


		return true;
	}
	return false;
}
#endif // !STANDALONE

