#pragma once

class TimeKeep
{
public:
	TimeKeep();

	float SceneTime()const; // Seconds
	float DeltaTime()const; //Seconds

	void Reset(); // Call before message loop
	void Start();
	void Stop();
	void Tick(); // Called Every frame



private:

	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPreviousTime;
	__int64 mCurrentTime;

	bool mIsStopped;

};


