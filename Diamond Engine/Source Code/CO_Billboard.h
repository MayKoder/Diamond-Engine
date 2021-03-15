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

	void SetAlignment(BILLBOARD_ALIGNMENT new_alignment);
	void Draw();
	Quat GetAlignment();
/*public:
	C_Transform* transform;
	ResourceMesh* res_mesh;

	float3 pos;*/
private:
	void UseAlignment();
	Quat ScreenAlign();
	void WorldAlign();
	void AxisAlign();

private:
	BILLBOARD_ALIGNMENT currentAlignment;
	//ResourceTexture* res_texture;	
};