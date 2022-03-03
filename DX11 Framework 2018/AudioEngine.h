#pragma once

#include "fmod.h"
#include <fmod.hpp>
#include <vector>

struct SoundStruct {
	const char* filePath;
	FMOD::Sound* sound;
	const char* name;
	//State machine state
};

class AudioEngine
{
public:
	AudioEngine();
	~AudioEngine() {

	}

	// === Core System Functions === // 
	void Update(float elapsed);
	void ErrorCheck(FMOD_RESULT);
	void ShutDown();
	float RandomBetween();

	// == Core Engine Functions == // 
	SoundStruct LoadSound(const char* name, const char* path);
	FMOD::Sound* CreateStream(const char* path);


private:

	FMOD::System* _system;

	// === Channels === //
	FMOD::Channel* _playingSongChannel;
	FMOD::ChannelGroup* _master;
	std::vector<FMOD::Channel*> _channels;
	
};

