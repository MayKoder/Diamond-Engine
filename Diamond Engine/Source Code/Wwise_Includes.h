#pragma once
#ifndef __WWISE_INCLUDES_H__
#define __WWISE_INCLUDES_H__

// Header files for the SDK interfaces
#include "Wwise/AK/SoundEngine/Common/AkSoundEngine.h"
#include "Wwise/AK/MusicEngine/Common/AkMusicEngine.h"

// Wwise memory management
#include "Wwise/AK/SoundEngine/Common/AkMemoryMgr.h"
#include "Wwise/AK/SoundEngine/Common/AkModule.h"
#include "Wwise/AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "Wwise/AK/Tools/Common/AkPlatformFuncs.h"

#include "Wwise/AK/SpatialAudio/Common/AkSpatialAudio.h"

// Libs
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkStreamMgr.lib")

#pragma comment(lib, "Wwise/Release(StaticCRT)/lib/AkSpatialAudio.lib")

//DirectX external libs							CHECK
#pragma comment(lib,"Wwise/dinput8.lib")
#pragma comment(lib,"Wwise/dsound.lib")
#pragma comment(lib,"Wwise/dxguid.lib")

namespace AK
{
	// Functions declared as extern in AkMemoryMgr.h
#ifdef WIN32
	void* AllocHook(size_t in_size);
	void FreeHook(void* in_ptr);
	void* VirtualAllocHook(void* in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_deProtect);
	void VirtualFreeHook(void* in_pMemAddress, size_t in_size, DWORD in_dwFreeType);
#endif // WIN32

}
#endif // !__WWISE_INCLUDES_H__