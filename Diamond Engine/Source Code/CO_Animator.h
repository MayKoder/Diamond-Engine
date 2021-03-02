#pragma once
#include "Component.h"
#include <iterator>
#include <vector>
#include <map>
#include <string>

#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/Quat.h"

class GameObject;
class Channel;
class ResourceAnimation;
class Resource;

typedef unsigned int uint;

struct AnimationClip
{
	AnimationClip();
	char name[32];
	float startFrame;
	float endFrame;
	bool loop;
	ResourceAnimation* originalAnimation;
};

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

	void AddAnimation(ResourceAnimation* anim);
	void AddClip(ResourceAnimation* anim);
	//void DrawLinkedBones() const;

	void Start();
	void Update() override;

	void UpdateChannelsTransform(const ResourceAnimation* settings, const ResourceAnimation* blend, float blendRatio);
	void UpdateMeshAnimation(GameObject* gameObject);

	float3 GetChannelPosition(const Channel& channel, float currentKey, float3 default) const;
	Quat GetChannelRotation(const Channel& channel, float currentKey, Quat default) const;
	float3 GetChannelScale(const Channel& channel, float currentKey, float3 default) const;

private:
	void DrawBones(GameObject*);
	ResourceAnimation* ClipToAnimation(AnimationClip clip);

public:
	void Play(std::string animName, float blendDuration);
	void Pause();
	void Resume();

public:
	GameObject* rootBone = nullptr;
	uint rootBoneUID;
	uint meshRendererUID;
	std::map<std::string, GameObject*> boneMapping;

	//Used for blending
	uint previous_animation = 0;
	uint current_animation = 0;
	bool playing = false;

private:
	bool started = false;
	bool showBones = false;

	float prevAnimTime = 0.0f;
	float time = 0.0f;
	float blendTime = 0.0f;
	float blendTimeDuration = 0.0f;

	float defaultBlend;

	bool channeIsLinked = false;
	bool bonesLinked = false;

	uint currentTimeAnimation = 0;
	uint previousTimeAnimation = 0;

	ResourceAnimation* _anim; 
	ResourceAnimation* currentAnimation = nullptr;
	ResourceAnimation* previousAnimation = nullptr;
	std::map<std::string,ResourceAnimation*> animations;
	std::vector<AnimationClip> clips;
	AnimationClip* selectedClip;
};

