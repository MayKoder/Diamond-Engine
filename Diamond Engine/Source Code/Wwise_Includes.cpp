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

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;

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
        assert(!"Could not create the Streaming Manager.");
        return false;
    }

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);
    if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
    {
        assert(!"Could not create the streaming device and Low-Level I/O system.");
        return false;
    }

    AkInitSettings initSettings;
    AkPlatformInitSettings platformInitSettings;
    AK::SoundEngine::GetDefaultInitSettings(initSettings);
    AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

    if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
    {
        assert(!"Could not initialize the Sound Engine.");
        return false;
    }

    AkMusicSettings musicInit;
    AK::MusicEngine::GetDefaultInitSettings(musicInit);
    if (AK::MusicEngine::Init(&musicInit) != AK_Success)
    {
        assert(!"Could not initialize the Music Engine.");
        return false;
    }

    // TODO: Check BasePath for different versions. What if there are banks in library as resources? Use assets or library
    g_lowLevelIO.SetBasePath(AKTEXT("Project Folder/Assets/SoundBanks/"));

    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    AkBankID bankID;
    AKRESULT ret = AK::SoundEngine::LoadBank("Init.bnk", bankID);
    if (ret != AK_Success)
    {
        assert(!"Could not initialize Init.bnk.");
        return false;
    }

    return true;
}

void TermSoundEngine()
{
    AK::SoundEngine::ClearBanks();

     // Terminate the music engine
    AK::MusicEngine::Term();

    // Terminate the sound engine
    AK::SoundEngine::Term();

   // that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
    g_lowLevelIO.Term();

    if (AK::IAkStreamMgr::Get())
        AK::IAkStreamMgr::Get()->Destroy();

    // Terminate the Memory Manager
    AK::MemoryMgr::Term();
}