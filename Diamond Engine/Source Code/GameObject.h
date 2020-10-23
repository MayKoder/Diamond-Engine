#pragma once

#include<vector>

class GameObject
{
public:
	GameObject();
	~GameObject();

	GameObject* parent;
	std::vector<GameObject*> children;

private:

};