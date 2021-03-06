#ifndef STANDALONE

#ifndef __W_SCENE_H__
#define __W_SCENE_H__

#include "Window.h"
#include"ImGuizmo/ImGuizmo.h"

#include "MathGeoLib/include/Math/float4x4.h"

class Application;

class W_Scene : public Window
{
public:
	W_Scene(Application* _app);
	virtual ~W_Scene();

	void InitSceneView();

	void Draw() override;

	ImVec2 NormalizeOnWindow(float x, float y, float w, float h, ImVec2 point);
	ImVec2 GetRegionAvailable();

private:
	void DrawGuizmo();

public:
	ImGuizmo::OPERATION operation;
	ImGuizmo::MODE mode;

	Application* App;

private:
	bool manipulatingGuizmo = false;

	float4x4 oldMat;
};

#endif //__W_SCENE_H__

#endif // !STANDALONE