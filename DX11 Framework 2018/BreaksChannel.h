#pragma once

#include "Vector3.h"
#include "fmod.h"
#include <fmod.hpp>
#include <bitset>

class BreaksEngine;
class BreaksCore;

//Defines the virtual settings a channel can use to select its virtualisation preference
enum class VirtualSetting
{
	RESTART, PAUSE, MUTE
};

struct BreaksChannel {
	BreaksChannel(class BreaksCore& breaksCore, int soundID, struct SoundData* soundData, VirtualSetting virtSetting, Vector3& pos, float volume);
	~BreaksChannel();

	//Defines the different states of the state machine
	enum class State {
		INIT,
		TOPLAY,
		PLAYING,
		LOADING,
		PREPLAYING,
		VIRTUALISING,
		VIRTUAL,
		STOPPING,
		STOPPED
	};

	enum class UpdateFlag : short {
		POSITION = 0,
		VOLUME
	};

	//Channel Wrapper Data Variables
	class BreaksCore& core;
	FMOD::Channel* channel = nullptr;
	SoundData* soundData;
	int soundID;


	//Virtualisation Settings
	VirtualSetting virtualSetting;

	bool virtualFlag = false;
	float virtualDistance;
	float virtualTimer = 0.0f;
	float virtualCheckPlayPeriod;
	float virtualCheckVirtualPeriod;
	float virtualFadeInTime;
	float virtualFadeOutTime;
	float stopFadeOutTime;

	State state = State::INIT;
	float volume = 1.0f;
	bool stopRequested = false;
	Vector3 position;

	//Virtual Flags
	bool isVirtFlagEffective;
	bool virtFlag = false;
	std::bitset<8> updateFlags;

	void SetUpdateFlag(UpdateFlag updateFlag, bool flag);
	void Update(float deltaTime);
	void UpdateParams();
	void RunFadeIn(float deltaTime);
	void RunFadeOut(float deltaTime);
	bool VirtualCheck(bool allowOneShot, float deltaTime);
	bool IsPlaying();
	bool IsOneShot();

};
