#include "CO_Transform2D.h"

#include "GameObject.h"


C_Transform2D::C_Transform2D(GameObject* gameObject) : Component(gameObject)
{

}


C_Transform2D::~C_Transform2D()
{

}


void C_Transform2D::Update()
{
	if (updateTransform == true)
		UpdateTransform();
}


#ifndef STANDALONE
bool C_Transform2D::OnEditor()
{
	//TODO Add modify pos logic
	return true;
}
#endif // !STANDALONE


void C_Transform2D::UpdateTransform()
{
	updateTransform = false;

	if (gameObject->parent != nullptr)
	{
		Component* parentTrans = gameObject->parent->GetComponent(Component::TYPE::TRANSFORM_2D);
		if (parentTrans != nullptr)
		{
			C_Transform2D* parentTransform = (C_Transform2D*)parentTrans;
			posX = localPosX + parentTransform->posX;
			posY = localPosY + parentTransform->posY;
			rotation = localRotation + parentTransform->rotation;
		}

		else
		{
			posX = localPosX;
			posY = localPosY;
			rotation = localRotation;
		}
	}

	int childCount = gameObject->children.size();
	for (int i = 0; i < childCount; ++i)
	{
		Component* childComponent = gameObject->children[i]->GetComponent(Component::TYPE::TRANSFORM_2D);

		if (childComponent != nullptr)
		{
			C_Transform2D* childTransform = (C_Transform2D*)childComponent;
			childTransform->UpdateTransform();
		}
	}
}


void C_Transform2D::SetTransform(float locPosX, float locPosY, float locRotation)
{
	localPosX = locPosX;
	localPosY = locPosY;
	localRotation = locRotation;

	if (gameObject->parent != nullptr)
	{
		if (gameObject->parent->GetComponent(Component::TYPE::CANVAS) == nullptr)
		{
			Component* parentTransformComp = gameObject->parent->GetComponent(Component::TYPE::TRANSFORM_2D);
			if (parentTransformComp != nullptr)
			{
				C_Transform2D* parentTransform = (C_Transform2D*)parentTransformComp;

				posX = localPosX + parentTransform->posX;
				posY = localPosY + parentTransform->posY;
				rotation = localRotation + parentTransform->rotation;
			}
		}
	}
}