#include "DETime.h"


float DETime::time;
float DETime::timeScale = 1.f;
float DETime::deltaTime;

int DETime::frameCount;

float DETime::realTimeSinceStartup;
float DETime::realTimeDeltaTime;
float DETime::realStartTime;

GameState DETime::state = GameState::STOP;
Timer DETime::gameTimer;

void DETime::Play()
{
	state = GameState::PLAY;
	gameTimer.Start();
}

void DETime::Stop()
{
	state = GameState::STOP;
	time = 0.f;
	deltaTime = 0.f;
	frameCount = 0;

	gameTimer.Stop();
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
		state = GameState::STEP;
}

void DETime::Resume()
{
	if (state != GameState::STEP)
		state = GameState::PLAY;

	gameTimer.Start();
}
