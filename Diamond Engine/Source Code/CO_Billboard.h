#pragma once

#include "Component.h"
#include "MathGeoLib/include/Math/Quat.h"

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

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	void SetAlignment(BILLBOARD_ALIGNMENT new_alignment);
	void Draw();
	Quat GetAlignment();

private:
	Quat ScreenAlign();
	Quat WorldAlign();
	Quat AxisAlign();

private:
	BILLBOARD_ALIGNMENT currentAlignment;
	bool horizontalAligned; //For axis alignment only
};