#pragma once

#include <map>
#include <vector>
#include <iterator>
#include "Vector3.h"
#include "fmod.h"
#include <fmod.hpp>
#include <bitset>

class BreaksChannel;
class BreaksEngine;

// ========================== Breaks Core ========================== //
//Breaks core contains the core functionaliy of the engine, including
//the state machine architecture and FMOD::Channel wrapper used to pass
//sounds through the state machine structure.
class BreaksCore {
public:
	BreaksCore();
	~BreaksCore();
	void SetEarPos(Vector3& pos, bool isRelative, Vector3 forward, Vector3 up);
	void Update(float deltaTime);
	bool CheckLoaded(int soundID);
	void LoadSound(int soundID);

	//Fmod system definition
	FMOD::System* system;

	Vector3 earPos;

	int nextSoundID;
	int nextBreakChannelID;

	//Define the maps for data to be stored in for the engine
	typedef std::map<int, FMOD::Sound*> SoundMap;
	typedef std::map<int, struct SoundData*> SoundDataMap;
	typedef std::map<int, std::unique_ptr<struct BreaksChannel>> BreaksChannelMap;

	SoundMap sounds;
	SoundDataMap soundDataMap;
	BreaksChannelMap channelMap;

private:

};


