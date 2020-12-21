#pragma once
#include"Timer.h"

enum class GameState {
	PLAY,
	PAUSE,
	STOP,
	STEP,
	PRESTEP,
};

//TODO: Would be cooler to be a namespace like unity, but static and externals are going wild
struct DETime
{

	//Seconds since game start
	static float time;

	//Scale of time 0.5 = 2 times slower
	static float timeScale;

	//Last frame completion time
	static float deltaTime;

	//Frames since game start
	static int frameCount;

	//Real time [DONE kinda]
	static float realTimeSinceStartup;
	static float realTimeDeltaTime;
	static float realStartTime;

	static GameState state/* = GameState::STOP*/;

	static void Play();
	static void Stop();
	static void Pause();
	static void Step();
	static void Resume();

	static void PreUpdate();
	static Timer gameTimer;

	static const char* GetStateString();
};