#pragma once


#include "fmod.h"
#include <fmod.hpp>
#include <vector>
#include <map>
#include <iostream>
#include "Vector3.h"

// ================================ Breaks Engine ================================== //
//The Breaks Engine is a state machine oriented audio engine for application
//in games, 3D or 2D. Consisting of BreaksCore, housing the state machine 
//architecture and core functionality, and the Breaks Engine where all of this 
//is wrapped and applied. The engine is designed to be expandable and maintainable,
//with its state machine design meaning new features in the form of states can be
//added at the developers desire. 
// ================================================================================ //

//class BreaksCore;

//SoundData contains basic sound data information for each BreaksChannel sound
struct SoundData {
	std::string fileName;
	float volume;
	float minDistance;
	float maxDistance;
	float virtualDistance;
	bool is3D;
	bool isLoop;
	bool isStream;
};


class BreaksEngine
{
public:
	BreaksEngine();
	~BreaksEngine() {}

	// === Core System Functions === // 
	void Initialize();
	void Update(float elapsed);
	//void ErrorCheck(FMOD_RESULT result);
	void SetEarPos(Vector3 pos, bool isRelative, Vector3 forward, Vector3 up);

	void GetOcclusion(FMOD_VECTOR* listenerPos, FMOD_VECTOR* sourcePos, float directOcclusion, float reverbOcclusion);
	
	void ShutDown();
	//float RandomBetween();

	// == Core Engine Functions == // 
	//SoundStruct LoadSound(const char* name, const char* path);
	//FMOD::Sound* CreateStream(const char* path);

	//void PlaySound(int SoundStruct sound, float volume /*position*/);
	int RegisterSound(SoundData& soundData, bool load);
	int PlayAudio(int soundID, Vector3 pos, float volume);
	
	void LoadSound(int soundID);
	void UnloadSound(int soundID);
	//void StopSound(int soundID);

	bool CheckLoaded(int soundID);
	void SetBreaksChannelVolume(int channelID, float volume);
	void SetSoundDirection(int channelID, Vector3 direction, Vector3 coneSize);
	void ChangeVirtualSetting(int channelID, int setting);
	void SetBreaksChannelPosition(int channelID, Vector3 pos, bool isRelative);
	void StopBreaksChannel(int channelID);
	void VirtualiseBreaksChannel(int channelID);
	void DeVirtualiseBreaksChannel(int channelID);
	FMOD::Reverb3D* CreateReverb(Vector3 position, FMOD_REVERB_PROPERTIES properties);
	void CreateFmodGeometry(FMOD::Geometry* geometry, int maxPoligons, int maxVertices);
};

