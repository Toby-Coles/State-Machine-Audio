#include "AudioEngine.h"

AudioEngine::AudioEngine()
{
	//Create and initiliaze the fmod core system
	FMOD::System_Create(&_system);
	
	FMOD_RESULT result;
	result = _system->init(50, FMOD_INIT_NORMAL, 0);
	ErrorCheck(result);

	//Create channels for each audio type
	_system->getMasterChannelGroup(&_master);
	//for (int i = 0; i < Type_Count; i++)
	//{
	//	_fmodSystem->createChannelGroup(0, &_groups[i]);
	//	_master->addGroup(_groups[i]);
	//}
	////Set up modes for each type
	//modes[Type_SFX] = FMOD_DEFAULT;
	//modes[Type_Song] = FMOD_DEFAULT | FMOD_CREATESTREAM | FMOD_LOOP_NORMAL;

	//_currentSongChannel = nullptr;
}


// ============ Core Engine Functions ============ // 

//Play a sound based off the SoundStruct object passed in
void AudioEngine::PlaySound(SoundStruct sound, float volume)
{
	FMOD::Channel* channel;
	FMOD_RESULT result = _system->playSound(sound.sound, _SFX, false, &channel);
	ErrorCheck(result);
	channel->setChannelGroup(_SFX);
	channel->setVolume(volume);
	channel->setPaused(false);
	_channels.push_back(channel);

}

//Each Fmod function returns an FMOD_RESULT, defining the error type it encountered if it did enocunter one 
void AudioEngine::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		std::cout << "Audio Error: : ";
		std::cout << result << std::endl;
	}
}


