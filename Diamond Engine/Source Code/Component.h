#pragma once

class Component
{
public:

	Component();
	virtual ~Component();

	virtual void Enable();
	virtual void Disable();

	virtual void Update();

private:

};