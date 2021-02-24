#include "Wwise_Includes.h"
#include <assert.h>

namespace AK
{
    void* AllocHook(size_t in_size)
    {
        return malloc(in_size);
    }

    void FreeHook(void* in_ptr)
    {
        free(in_ptr);
    }

    void* VirtualAllocHook(
        void* in_pMemAddress,
        size_t in_size,
        DWORD in_dwAllocationType,
        DWORD in_dwProtect
    )
    {
        return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
    }
    void VirtualFreeHook(
        void* in_pMemAddress,
        size_t in_size,
        DWORD in_dwFreeType
    )
    {
        VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
    }
}

bool InitSoundEngine()
{
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);
    if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
    {
        assert(!"Could not create the memory manager.");
        return false;
    }

    AkStreamMgrSettings stmSettings;
    AK::StreamMgr::GetDefaultSettings(stmSettings);
    if (!AK::StreamMgr::Create(stmSettings))
    {
        assert(!"Could not create the Streaming Manager");
        return false;
    }

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);        /// Add low level I/O



    return true;
}