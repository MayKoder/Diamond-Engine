#pragma once

#include "Component.h"
#include"MathGeoLib/include/Geometry/Frustum.h"
#include"MathGeoLib/include/Math/float4x4.h"

class ResourceShader;
class ResourceMaterial;

class C_DirectionalLight : public Component
{
public:
	C_DirectionalLight(GameObject* _gm);
	virtual ~C_DirectionalLight();

	void Update() override;

#ifndef STANDALONE
	bool OnEditor() override;
#endif // !STANDALONE

	static inline TYPE GetType() { return TYPE::DIRECTIONAL_LIGHT; }; //This will allow us to get the type from a template

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	void StartPass();
	void PushLightUniforms(ResourceMaterial* material);
	void EndPass();

	Frustum orthoFrustum;
	ResourceShader* depthShader;
	float4x4 spaceMatrixOpenGL;

	float2 orthoSize;

private:
	unsigned int depthMapFBO;	//Framebuffer
	unsigned int depthMap;		//Depth texture

	float3 lightColor;
};