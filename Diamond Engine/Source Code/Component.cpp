#include "Component.h"

Component::Component(GameObject* _gm) : active(true), /*name(""),*/ gameObject(_gm), type(Type::None)
{
}

Component::~Component()
{
}

void Component::Enable()
{
}

void Component::Disable()
{
}

void Component::Update()
{
}

void Component::OnEditor()
{

}
