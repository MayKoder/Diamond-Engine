#include "MO_AudioManager.h"
#include "Wwise_Includes.h"
#include "Wwise/low_level_IO/Win32/AkFilePackageLowLevelIOBlocking.h"


#include <assert.h>

CAkFilePackageLowLevelIOBlocking g_lowLevelIO;


ModuleAudioManager::ModuleAudioManager(Application* app, bool start_enabled): Module(app,start_enabled)
{
	//TODO listener code here
}

ModuleAudioManager::~ModuleAudioManager()
{	
	//TODO listener code here
}

bool ModuleAudioManager::Init()
{
	//Init Memory Manager
	AkMemSettings memSettings;

	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(!"Could not create the memory manager.");
		return false;
	}

	//Init Stream Manager
	AkStreamMgrSettings stmSettings;

	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Customize the Stream Manager settings here.

	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager.");
		return false;
	}

	// Create a streaming device with blocking low-level I/O handshaking.
	AkDeviceSettings deviceSettings;

	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system.");
		return false;
	}

	//Init Sound Engine
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;

	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}

	//Init Music Engine
	AkMusicSettings musicInit;

	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}

	//Load Soundbanks
	// TODO: Check BasePath for different versions. What if there are banks in library as resources? Use assets or library
	g_lowLevelIO.SetBasePath(AKTEXT("Assets/SoundBanks/"));

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

bool ModuleAudioManager::Start()
{
	return true;
}

update_status ModuleAudioManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleAudioManager::PostUpdate(float dt)
{

	// Process bank requests, events, positions, RTPC, etc.
	AK::SoundEngine::RenderAudio();

	return UPDATE_CONTINUE;
}

bool ModuleAudioManager::CleanUp()
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



	return true;
}

void ModuleAudioManager::RegisterNewAudioObject(unsigned int id)
{
	AK::SoundEngine::RegisterGameObj(id);
}

void ModuleAudioManager::UnRegisterAudioObject(unsigned int id)
{
	AK::SoundEngine::UnregisterGameObj(id);
}

void ModuleAudioManager::StopAllSounds() const
{
	AK::SoundEngine::StopAll();
}

void ModuleAudioManager::PauseAllSounds() const
{
	//TODO I did this from wise
}

void ModuleAudioManager::ResumeAllSounds() const
{
	//TODO I did this from wise
}

bool ModuleAudioManager::LoadBank(std::string& name)
{
	AkBankID id;
	AKRESULT eResult;

	eResult = AK::SoundEngine::LoadBank(name.c_str(), id);
	if (eResult == AK_Success)
	{
		LOG(LogType::L_NORMAL,"Bank 'Main.bnk' has been loaded successfully");
		return true;
	}
	else
	{
		LOG(LogType::L_ERROR,"Error loading 'Main.bnk'");
		return false;
	}

}