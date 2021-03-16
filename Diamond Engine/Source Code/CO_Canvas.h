#ifndef __CO_CANVAS_H__
#define __CO_CANVAS_H__

#include "Component.h"

class C_Canvas : public Component
{
public:
	C_Canvas(GameObject* gameObject);
	~C_Canvas() override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE
};
#endif // !__CO_CANVAS_H__