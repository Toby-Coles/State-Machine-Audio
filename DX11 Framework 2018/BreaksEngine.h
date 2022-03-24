#pragma once

#include "fmod.h"
#include <fmod.hpp>
#include <vector>
#include <iostream>

// ========================== Breaks Engine ========================== //
//BreaksEngine houses the remaining logic and primary interaction point
//with the engine, wrapping BreakCores functionality to be used 

//struct SoundStruct {
//	const char* filePath;
//	FMOD::Sound* sound;
//	const char* name;
//	//State machine state
//};

class BreaksEngine
{
public:
	BreaksEngine();
	~BreaksEngine() {

	}

	// === Core System Functions === // 
	void Init();
	void Update(float elapsed);
	void ErrorCheck(FMOD_RESULT result);
	void ShutDown();
	float RandomBetween();

	//SoundData contains basic sound data information for each BreaksChannel sound
	struct SoundData {
		std::string name;
		float volume;
		float minDistance;
		float maxDistance;
		float virtualDistance;
		bool is3D;
		bool isLoop;
		bool isStream;
	};

	// == Core Engine Functions == // 
	//SoundStruct LoadSound(const char* name, const char* path);
	FMOD::Sound* CreateStream(const char* path);

	//void PlaySound(int SoundStruct sound, float volume /*position*/);
	int PlaySound(int soundID, Vec3 pos, float volume);
	void LoadSound(int soundID);
	void UnloadSound(int soundID);
	void StopSound(int soundID);
	bool CheckLoaded(int soundID);
	void SetBreaksChannelVolume(int channelID, float volume);
	void SetBreaksChannelPosition(int channelID, Vec3 pos, bool isRelative);
	void StopBreaksChannel();
	void VirtualiseBreaksChannel();
	void DeVirtualiseBreaksChannel();


private:

	//FMOD::System* _system;

	//// === Channels === //
	//FMOD::Channel* _playingSongChannel;
	//FMOD::ChannelGroup* _master;
	//FMOD::ChannelGroup* _SFX;
	//FMOD::ChannelControl* _soundTrack;

	//std::vector<FMOD::Channel*> _channels;
	
};

