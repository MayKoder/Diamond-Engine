#pragma once

#include "Component.h"

enum class BILLBOARDALIGNMENT {
	SCREEN_ALIGNED = 0,
	WORLD_ALIGNED,
	AXIS_ALIGNED
};

class C_Billboard : public Component {
	C_Billboard(GameObject* _gm);
	~C_Billboard();

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	void SetAlignment(BILLBOARDALIGNMENT new_alignment);
private:
	void ScreenAlign();
	void WorldAlign();
	void AxisAlign();

private:
	BILLBOARDALIGNMENT currentAlignment;
};