#include "Application.h"
#include "DEResource.h"
#include "IM_FileSystem.h"
#include "MO_ResourceManager.h"
#include "RE_Animation.h"
#include "IM_AnimationImporter.h"

ResourceAnimation::ResourceAnimation(unsigned int _uid) : Resource(_uid, Resource::Type::ANIMATION), animationName("No Name"),duration(0),ticksPerSecond(0), loop(true), time(0.0f)
{
}

ResourceAnimation::~ResourceAnimation()
{
	channels.clear();
}

bool ResourceAnimation::LoadToMemory()
{
	LOG(LogType::L_WARNING, "Animation loaded to memory");

	LoadCustomFormat(GetLibraryPath());

	return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	return false;
}

uint ResourceAnimation::SaveCustomFormat(ResourceAnimation* animation, char** buffer)
{
	uint size = 
		32 //name size 
		+ sizeof(float) //duration
		+ sizeof(float) //ticks per second
		+ sizeof(uint); //channels size

	//Channels size 
	std::map<std::string, Channel>::const_iterator it;
	for (it = animation->channels.begin(); it != animation->channels.end(); ++it)
		size += AnimationLoader::GetChannelsSize(it->second);

	//Allocate buffer size
	*buffer = new char[size];
	char* cursor = *buffer;

	//Name
	memcpy(cursor, &animation->animationName, 32);
	cursor += sizeof(animationName);

	//Duration
	memcpy(cursor, &animation->duration, sizeof(float));
	cursor += sizeof(float);

	//Ticks per second
	memcpy(cursor, &animation->ticksPerSecond, sizeof(float));
	cursor += sizeof(float);

	//Channels number
	uint channelsSize = animation->channels.size();
	memcpy(cursor, &channelsSize, sizeof(uint));
	cursor += sizeof(uint);

	//Save each channel data
	for (it = animation->channels.begin(); it != animation->channels.end(); ++it)
		AnimationLoader::SaveChannels(it->second, &cursor);

	return size;
}

void ResourceAnimation::LoadCustomFormat(const char* path)
{
	char* buffer = nullptr;

	uint size = FileSystem::LoadToBuffer(path, &buffer);

	const char* cursor = buffer;
	uint bytes;

	//Name
	memcpy(&animationName, cursor, 32);
	cursor += sizeof(animationName);

	//Fills Duration
	memcpy(&duration, cursor, sizeof(float));
	cursor += sizeof(float);

	//Fills Ticks per second
	memcpy(&ticksPerSecond, cursor, sizeof(float));
	cursor += sizeof(float);

	//Fills Channels number
	uint channelsSize = 0;
	memcpy(&channelsSize, cursor, sizeof(uint));
	cursor += sizeof(uint);

	//Fills channels
	for (uint i = 0; i < channelsSize; ++i)
	{
		Channel Channelat;
		AnimationLoader::LoadChannels(Channelat, &cursor);
		channels[Channelat.boneName.c_str()] = Channelat;
	}

	RELEASE_ARRAY(buffer);
}

std::map<std::string, Channel> ResourceAnimation::GetAllChannelsInRange(float startFrame, float endFrame)
{
	std::map<std::string, Channel> channelsInRange;

	std::map<std::string, Channel>::iterator channel_it = channels.begin();
	for (channel_it; channel_it != channels.end(); channel_it++)
	{
		for (std::map<double, float3>::iterator position_key = channel_it->second.positionKeys.begin(); position_key != channel_it->second.positionKeys.end(); ++position_key) {
			if (position_key->first >= startFrame) 
			{
				if (position_key->first < endFrame)  //Frame in range 
				{
					//channelsInRange[channel_it->first].positionKeys = position_key;
				}
				else //Frame out of range, needs to create intermediate frame
				{

				}
			}
		}

		for (std::map<double, Quat>::iterator rotation_key = channel_it->second.rotationKeys.begin(); rotation_key != channel_it->second.rotationKeys.end(); ++rotation_key) {

		}

		for (std::map<double, float3>::iterator scale_key = channel_it->second.scaleKeys.begin(); scale_key != channel_it->second.scaleKeys.end(); ++scale_key) {

		}
	}

	return channelsInRange;
}

Channel::~Channel()
{
	boneName.clear();
	positionKeys.clear();
	rotationKeys.clear();
	scaleKeys.clear();
}

std::map<double, float3>::const_iterator Channel::GetPrevPosKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = positionKeys.lower_bound(currentKey);
	if (ret != positionKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextPosKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = positionKeys.find(currentKey);
	if (ret == positionKeys.end()) {
		ret = positionKeys.upper_bound(currentKey);

		if (ret == positionKeys.end())
			ret--;
	}
	return ret;
}

std::map<double, Quat>::const_iterator Channel::GetPrevRotKey(double currentKey) const
{
	std::map<double, Quat>::const_iterator ret = rotationKeys.lower_bound(currentKey);
	if (ret != rotationKeys.begin())
		ret--;
	return ret;
}

std::map<double, Quat>::const_iterator Channel::GetNextRotKey(double currentKey) const
{
	std::map<double, Quat>::const_iterator ret = rotationKeys.find(currentKey);
	if (ret == rotationKeys.end()) {
		ret = rotationKeys.upper_bound(currentKey);

		if (ret == rotationKeys.end())
			ret--;
	}
	return ret;
}

std::map<double, float3>::const_iterator Channel::GetPrevScaleKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = scaleKeys.lower_bound(currentKey);
	if (ret != scaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::const_iterator Channel::GetNextScaleKey(double currentKey) const
{
	std::map<double, float3>::const_iterator ret = scaleKeys.find(currentKey);
	if (ret == scaleKeys.end()) {
		ret = scaleKeys.upper_bound(currentKey);

		if (ret == scaleKeys.end())
			ret--;
	}

	return ret;
}
