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

void AudioEngine::ErrorCheck(FMOD_RESULT)
{
}
