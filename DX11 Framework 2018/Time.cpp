
#include "Time.h"
#include <Windows.h>

TimeKeep::TimeKeep() : mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0), mPausedTime(0), mPreviousTime(0), mCurrentTime(0), mIsStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	 mSecondsPerCount = 1.0 / (double)countsPerSec;
}

float TimeKeep::SceneTime() const
{
	if (mIsStopped)
	{
		return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)(((mCurrentTime - mPausedTime) - mBaseTime) * mSecondsPerCount);

	}
}

float TimeKeep::DeltaTime() const
{

	return (float)mDeltaTime;
}

void TimeKeep::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	mBaseTime = currentTime;
	mPreviousTime = currentTime;
	mStopTime = 0.0;
	mIsStopped = false;

}

void TimeKeep::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);
	
	//Accumulate time elapsed between stop and start pairs

	if (mIsStopped)
	{
		//Then accumulate stop time
		mPausedTime += (startTime - mStopTime);

		//since we are starting the timer back up, the current previous time is not valid
		mPreviousTime = startTime;
		mStopTime = 0;
		mIsStopped = false;
	}

}

//Stop the timer
void TimeKeep::Stop()
{
	if (!mIsStopped)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
		mStopTime = currentTime;
		mIsStopped = true;
	}
}

//Timer tick, updated every frame and keeps the timer increasing
void TimeKeep::Tick()
{
	if (mIsStopped)
	{
		mDeltaTime = 0.0f;
		return;
	}

	//Get the current time this frame
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mCurrentTime = currentTime;

	//Time difference between current frame and the previous
	mDeltaTime = (mCurrentTime - mPreviousTime) * mSecondsPerCount;

	//Prepare for next frame
	mPreviousTime = mCurrentTime;

	//Force nonnegative incase processor entors power saver or get shuffled to another processor
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}



}




