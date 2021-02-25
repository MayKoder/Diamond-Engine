#pragma once
#include "Component.h"
#include <iterator>
#include <vector>
#include <map>
#include <string>

#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/float3.h"
#include"MathGeoLib/include/Math/Quat.h"

class GameObject;
class Channel;
class ResourceAnimation;
class Resource;

typedef unsigned int uint;

class C_Animator : public Component
{
public:
	C_Animator(GameObject* gameObject);
	~C_Animator();

	void SetResource(ResourceAnimation* re_anim);

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	bool OnEditor() override;

	void LinkChannelBones(GameObject* gameObject);
	void StoreBoneMapping(GameObject* gameObject);

	void SetAnimation(ResourceAnimation* anim);
	//void DrawLinkedBones() const;

	void Start();
	void Update() override;

	void UpdateChannelsTransform(const ResourceAnimation* settings, const ResourceAnimation* blend, float blendRatio);
	void UpdateMeshAnimation(GameObject* gameObject);

	float3 GetChannelPosition(const Channel& channel, float currentKey, float3 default) const;
	Quat GetChannelRotation(const Channel& channel, float currentKey, Quat default) const;
	float3 GetChannelScale(const Channel& channel, float currentKey, float3 default) const;

	//void SetAnimation(const char* name, float blendTime = 0.0f);
	//void SetAnimation(uint index, float blendTime = 0.0f);

	//ResourceAnimation* GetAnimation(uint index);

	//void SetResource(Resource* resource);
	//void SetResource(unsigned long long ID);

	////uint64 GetResourceID() const;
	//static inline Type GetType() { return Type::Animator; };

private:
	void DrawBones(GameObject*);

public:
	GameObject* rootBone = nullptr;
	std::map<std::string, GameObject*> boneMapping;

	//Used for blending
	uint previous_animation = 0;
	uint current_animation = 0;
	bool playing = false;
	void Pause();
	void Resume();
private:
	bool started = false;

	float prevAnimTime = 0.0f;
	float time = 0.0f;
	float blendTime = 0.0f;
	float blendTimeDuration = 0.0f;

	bool channeIsLinked = false;
	bool bonesLinked = false;

	uint currentTimeAnimation = 0;
	uint previousTimeAnimation = 0;

private:
	ResourceAnimation* _anim;
	ResourceAnimation* currentAnimation = nullptr;
	ResourceAnimation* previousAnimation = nullptr;
	std::vector<ResourceAnimation*> animations;
};

