#include "IM_AnimationImporter.h"
#include "Application.h"
#include "MO_Editor.h"

#include "GameObject.h"
#include "CO_Transform.h"
#include "CO_Animator.h"

#include "RE_Animation.h"

#include "IM_FileSystem.h"
#include "IM_ModelImporter.h"

#include "MO_Scene.h"
#include "MO_ResourceManager.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"
#include "Assimp/include/vector3.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc142-mt.lib")


void  AnimationLoader::logCallback(const char* message, char* user)
{
	EngineExternal->moduleEditor->LogToConsole(message);
}

//uint AnimationLoader::SaveCustomFormat(ResourceAnimation* animation, char** buffer)
//{
//	uint size = sizeof(float) + sizeof(float) + sizeof(uint);
//
//	//Channels size 
//	std::map<std::string, Channel>::const_iterator it;
//	for (it = animation->channels.begin(); it != animation->channels.end(); ++it)
//		size += AnimationLoader::GetChannelsSize(it->second);
//
//	//Allocate buffer size
//	*buffer = new char[size];
//	char* cursor = *buffer;
//
//	//Duration
//	memcpy(cursor, &animation->duration, sizeof(float));
//	cursor += sizeof(float);
//
//	//Ticks per second
//	memcpy(cursor, &animation->ticksPerSecond, sizeof(float));
//	cursor += sizeof(float);
//
//	//Channels number
//	uint channelsSize = animation->channels.size();
//	memcpy(cursor, &channelsSize, sizeof(uint));
//	cursor += sizeof(uint);
//
//	//Save each channel data
//	for (it = animation->channels.begin(); it != animation->channels.end(); ++it)
//		AnimationLoader::SaveChannels(it->second, &cursor);
//
//	return size;
//}
//
//ResourceAnimation* AnimationLoader::LoadCustomFormat(const char* path)
//{
//	char* buffer = nullptr;
//
//	uint size = FileSystem::LoadToBuffer(path, &buffer);
//
//	const char* cursor = buffer;
//	uint bytes;
//
//	//Empty animation
//	ResourceAnimation* anim = dynamic_cast<ResourceAnimation*>(EngineExternal->moduleResources->CreateNewResource("", 0, Resource::Type::ANIMATION));
//
//	//Fills Duration
//	memcpy(&anim->duration, cursor, sizeof(float));
//	cursor += sizeof(float);
//
//	//Fills Ticks per second
//	memcpy(&anim->ticksPerSecond, cursor, sizeof(float));
//	cursor += sizeof(float);
//
//	//Fills Channels number
//	uint channelsSize = 0;
//	memcpy(&channelsSize, cursor, sizeof(uint));
//	cursor += sizeof(uint);
//
//	//Fills channels
//	for (uint i = 0; i < channelsSize; ++i)
//	{
//		Channel Channelat;
//		AnimationLoader::LoadChannels(Channelat, &cursor);
//		anim->channels[Channelat.boneName.c_str()] = Channelat;
//	}
//	return anim;
//}

ResourceAnimation* AnimationLoader::LoadAnimation(aiAnimation* importedAnimation, uint oldUID)
{
	uint UID = oldUID;
	if (UID == 0)
		UID = EngineExternal->moduleResources->GenerateNewUID();

	LOG(LogType::L_NORMAL, "%s", importedAnimation->mName.C_Str());
	std::string file = ANIMATIONS_PATH;
	file += std::to_string(UID);
	file += ".anim";

	ResourceAnimation* anim = dynamic_cast<ResourceAnimation*>(EngineExternal->moduleResources->CreateNewResource("", UID, Resource::Type::ANIMATION));

	anim->ticksPerSecond = importedAnimation->mTicksPerSecond;
	anim->duration = importedAnimation->mDuration;

	//Import from Assimp
	for (int i = 0; i < importedAnimation->mNumChannels; i++)
	{
		Channel channel;

		channel.boneName = importedAnimation->mChannels[i]->mNodeName.C_Str();
		uint pos = channel.boneName.find("_$AssimpFbx$_");
		if (pos != std::string::npos)
		{
			channel.boneName = channel.boneName.substr(0, pos);
		}
		for (int j = 0; j < importedAnimation->mChannels[i]->mNumPositionKeys; j++)
		{
				aiVector3t<float> aiValue = importedAnimation->mChannels[i]->mPositionKeys[j].mValue;
				float3 positionKey = float3(aiValue.x, aiValue.y, aiValue.z);

				channel.positionKeys[importedAnimation->mChannels[i]->mPositionKeys[j].mTime] = positionKey;
			
		}

		for (int j = 0; j < importedAnimation->mChannels[i]->mNumRotationKeys; j++)
		{
			aiQuaterniont<float> aiValue = importedAnimation->mChannels[i]->mRotationKeys[j].mValue;
			Quat rotationKey = Quat(aiValue.x, aiValue.y, aiValue.z, aiValue.w);

			channel.rotationKeys[importedAnimation->mChannels[i]->mRotationKeys[j].mTime] = rotationKey;
		}

		for (int j = 0; j < importedAnimation->mChannels[i]->mNumScalingKeys; j++)
		{
			aiVector3t<float> aiValue = importedAnimation->mChannels[i]->mScalingKeys[j].mValue;
			float3 scaleKey = float3(aiValue.x, aiValue.y, aiValue.z);

			channel.scaleKeys[importedAnimation->mChannels[i]->mScalingKeys[j].mTime] = scaleKey;
		}
		anim->channels[channel.boneName] = channel;
	}

	//Save animation own format
	char* buffer;
	uint size = anim->SaveCustomFormat(anim, &buffer);

	FileSystem::Save(file.c_str(), buffer, size, false);

	std::string file_name = std::to_string(UID);
	file_name += ".anim";

	RELEASE_ARRAY(buffer);
	//FileSystem::Load(ANIMATIONS_PATH, file_name.c_str(), &buffer);
	// 
	//ResourceAnimation* loaded_animation =LoadCustomFormat(buffer);

	return anim;
}

uint AnimationLoader::GetChannelsSize(const Channel& channel)
{
	uint ret = sizeof(uint) + channel.boneName.size() + sizeof(uint) * 3;
	//Translation
	ret += sizeof(double) * channel.positionKeys.size() + sizeof(float) * channel.positionKeys.size() * 3;
	//Rotation
	ret += sizeof(double) * channel.rotationKeys.size() + sizeof(float) * channel.rotationKeys.size() * 4;
	//Scale
	ret += sizeof(double) * channel.scaleKeys.size() + sizeof(float) * channel.scaleKeys.size() * 3;

	return ret;
}

void AnimationLoader::SaveChannels(const Channel& channel, char** cursor)
{
	uint nameSize = channel.boneName.size();
	memcpy(*cursor, &nameSize, sizeof(uint));
	*cursor += sizeof(uint);

	memcpy(*cursor, channel.boneName.c_str(), channel.boneName.size());
	*cursor += channel.boneName.size();

	//Ranges
	uint ranges[3] = { channel.positionKeys.size(), channel.rotationKeys.size(), channel.scaleKeys.size() };
	memcpy(*cursor, ranges, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	//Save each channel, depending on float or quat based
	SaveChanKeys(channel.positionKeys, cursor);
	SaveChanKeys(channel.rotationKeys, cursor);
	SaveChanKeys(channel.scaleKeys, cursor);
}

void AnimationLoader::SaveChanKeys(const std::map<double, float3>& map, char** cursor)
{
	std::map<double, float3>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;
	}
}

void AnimationLoader::SaveChanKeys(const std::map<double, Quat>& map, char** cursor)
{
	std::map<double, Quat>::const_iterator it = map.begin();
	for (it = map.begin(); it != map.end(); it++)
	{
		memcpy(*cursor, &it->first, sizeof(double));
		*cursor += sizeof(double);

		memcpy(*cursor, &it->second, sizeof(float) * 4);
		*cursor += sizeof(float) * 4;
	}
}

void AnimationLoader::LoadChannels(Channel& channel, const char** cursor)
{
	uint bytes = 0;

	uint nameSize = 0;
	memcpy(&nameSize, *cursor, sizeof(uint));
	*cursor += sizeof(uint);

	if (nameSize > 0)
	{
		char* string = new char[nameSize + 1];
		bytes = sizeof(char) * nameSize;
		memcpy(string, *cursor, bytes);
		*cursor += bytes;
		string[nameSize] = '\0';
		channel.boneName = string;
		RELEASE_ARRAY(string);
	}

	//Ranges
	uint ranges[3];
	memcpy(&ranges, *cursor, sizeof(uint) * 3);
	*cursor += sizeof(uint) * 3;

	LoadChanKeys(channel.positionKeys, cursor, ranges[0]);
	LoadChanKeys(channel.rotationKeys, cursor, ranges[1]);
	LoadChanKeys(channel.scaleKeys, cursor, ranges[2]);
}

void AnimationLoader::LoadChanKeys(std::map<double, float3>& map, const char** cursor, uint size)
{
	for (uint i = 0; i < size; i++)
	{
		double time;
		memcpy(&time, *cursor, sizeof(double));
		*cursor += sizeof(double);
		float data[3];
		memcpy(&data, *cursor, sizeof(float) * 3);
		*cursor += sizeof(float) * 3;

		map[time] = float3(data);
	}
}

void AnimationLoader::LoadChanKeys(std::map<double, Quat>& map, const char** cursor, uint size)
{
	for (uint i = 0; i < size; i++)
	{
		double time;
		memcpy(&time, *cursor, sizeof(double));
		*cursor += sizeof(double);
		float data[4];
		memcpy(&data, *cursor, sizeof(float) * 4);
		*cursor += sizeof(float) * 4;

		map[time] = Quat(data);
	}
}

void AnimationLoader::SetAnimationOnGameObjectRoot(aiAnimation** animArray, std::vector<ResourceAnimation*>& _sceneAnimations, GameObject* gmRoot)
{
	for (int i = 0; i < _sceneAnimations.size(); i++)
	{
		aiAnimation* importedAnim = animArray[i];

		if (importedAnim->mDuration != 0)
		{
			C_Animator* animator = dynamic_cast<C_Animator*>(gmRoot->AddComponent(Component::Type::Animator));
			animator->SetResource(_sceneAnimations[i]);
		}
	}
}
