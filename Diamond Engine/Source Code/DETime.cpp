#include "DETime.h"
#include "Application.h"
#include "MO_Scene.h"

int DETime::frameCount;

float DETime::time;
float DETime::timeScale = 1.f;
float DETime::deltaTime;
float DETime::realStartTime;
float DETime::realTimeDeltaTime;
float DETime::realTimeSinceStartup;

Timer DETime::gameTimer;

#ifndef STANDALONE
GameState DETime::state = GameState::STOP;
#else
GameState DETime::state = GameState::PLAY;
#endif // !STANDALONE

void DETime::Play()
{
	state = GameState::PLAY;
	gameTimer.Start();

	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void DETime::Stop()
{
	state = GameState::STOP;

	for (size_t i = 0; i < EngineExternal->moduleScene->activeScriptsVector.size(); ++i)
	{
		EngineExternal->moduleScene->activeScriptsVector[i]->OnApplicationQuit();
	}

	time = 0.f;
	deltaTime = 0.f;
	frameCount = 0;

	gameTimer.Stop();

	SDL_SetRelativeMouseMode(SDL_FALSE);
}

void DETime::PreUpdate()
{

	if (state == GameState::PLAY || state == GameState::STEP) {
		frameCount++;
		time += realTimeDeltaTime;
		deltaTime = realTimeDeltaTime * timeScale;
	}
	
	switch (state)
	{
	case GameState::PAUSE:
		deltaTime = 0.f;
		break;
	case GameState::STOP:
		time = 0.0f;
		break;
	case GameState::PRESTEP:
		state = GameState::STEP;
		break;
	case GameState::STEP:
		time += realTimeDeltaTime;
		deltaTime = realTimeDeltaTime * timeScale;
		state = GameState::PAUSE;
		break;
	}
}

const char* DETime::GetStateString()
{
	char* ret = "";
	switch (state)
	{
	case GameState::PLAY:
		ret = "Play";
		break;
	case GameState::PAUSE:
		ret = "Pause";
		break;
	case GameState::STOP:
		ret = "Stop";
		break;
	case GameState::STEP:
		ret = "Step";
		break;
	default:
		break;
	}

	return ret;
}

void DETime::Pause()
{
	if (state == GameState::PAUSE)
	{
		Resume();
		return;
	}

	if (state == GameState::PLAY || state == GameState::STEP)
	{
		state = GameState::PAUSE;
		gameTimer.Stop();
	}
}

void DETime::Step()
{
	if (state == GameState::PAUSE)
		state = GameState::PRESTEP;
}

void DETime::Resume()
{
	if (state != GameState::STEP)
		state = GameState::PLAY;

	gameTimer.Start();
}
