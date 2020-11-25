#pragma once


class DETime {

	//Seconds since game start
	float time;

	//Scale of time 0.5 = 2 times slower
	float timeScale;

	//Last frame completion time
	float deltaTime;

	//Frames since game start
	int frameCount; 


	float realTimeSinceStart;
	float RealTimeDeltaTime;



};