#pragma once
#include <vector>
#include <map>
#include "RE_Animation.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/float3.h"

class ResourceAnimation;
class aiNode;
class aiAnimation;
class GameObject;

typedef unsigned int uint;

namespace AnimationLoader
{
	void logCallback(const char* message, char* user);

	ResourceAnimation* ImportAnimation(aiAnimation* importedAnimation, uint oldUID = 0);
	uint GetChannelsSize(const Channel& channel);

	void SaveChannels(const Channel& channel, char** cursor);
	void SaveChanKeys(const std::map<double, float3>& map, char** cursor);
	void SaveChanKeys(const std::map<double, Quat>& map, char** cursor);

	void LoadChannels(Channel& channel, const char** cursor);
	void LoadChanKeys(std::map<double, float3>& map, const char** cursor, uint size);
	void LoadChanKeys(std::map<double, Quat>& map, const char** cursor, uint size);

	void SetAnimationOnGameObjectRoot(aiAnimation** animArray, std::vector<ResourceAnimation*>& _sceneAnimations, GameObject* gmRoot);
}