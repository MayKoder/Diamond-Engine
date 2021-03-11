#pragma once

#include "Component.h"

enum class BILLBOARD_ALIGNMENT : int
{
	SCREEN_ALIGNED = 0,
	WORLD_ALIGNED,
	AXIS_ALIGNED
};

class C_Billboard : public Component {
public:
	C_Billboard(GameObject* _gm);
	~C_Billboard();

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void SetAlignment(BILLBOARD_ALIGNMENT new_alignment);
	void Draw();
private:
	void UseAlignment();
	void ScreenAlign();
	void WorldAlign();
	void AxisAlign();

private:
	BILLBOARD_ALIGNMENT currentAlignment;
};