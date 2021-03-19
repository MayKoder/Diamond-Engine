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

	void Start();
	void Update() override;

	void SaveData(JSON_Object* nObj) override;
	void LoadData(DEConfig& nObj) override;

	void OnRecursiveUIDChange(std::map < uint, GameObject*> gameObjects) override;

#ifndef STANDALONE
	bool OnEditor() override;
	void SaveAnimation(ResourceAnimation* animation, const char* name);
#endif // !STANDALONE

	void StoreBoneMapping(GameObject* gameObject);

	void AddAnimation(ResourceAnimation* anim);
	void AddClip(ResourceAnimation* anim);

	void UpdateChannelsTransform(const ResourceAnimation* settings, const ResourceAnimation* blend, float blendRatio);

	float3	GetChannelPosition(const Channel& channel, float currentKey, float3 default) const;
	Quat	GetChannelRotation(const Channel& channel, float currentKey, Quat default) const;
	float3	GetChannelScale(const Channel& channel, float currentKey, float3 default) const;

	bool FindRootBone();

private:
	void DrawBones(GameObject*);
	ResourceAnimation* ClipToAnimation(AnimationClip clip);

public:
	void Play(std::string animName, float blendDuration = 0.3f);
	void Pause();
	void Resume();

public:
	GameObject* rootBone = nullptr;
	uint rootBoneUID;
	uint meshRendererUID;
	std::map<std::string, GameObject*> boneMapping;

	//Used for blending
	bool playing;

private:
	bool started;
	bool showBones;

	float prevAnimTime;
	float time;
	float blendTime;
	float blendTimeDuration;

	float defaultBlend;

	uint currentTimeAnimation;
	uint previousTimeAnimation;

	ResourceAnimation* currentAnimation;
	ResourceAnimation* previousAnimation;
	std::map<std::string,ResourceAnimation*> animations;
	std::vector<AnimationClip> clips;
	AnimationClip* selectedClip;
};

