#include "BreaksCore.h"
#include "BreaksEngine.h"
#include "BreaksChannel.h"

// ================================ BreaksCore Function Definitions ================================ // 
// ====== BreaksCore Constructer Definition ====== //
BreaksCore::BreaksCore() : earPos{ 0.0f, 0.0f, 0.0f }, nextSoundID(0), nextBreakChannelID(0)
{
	system = nullptr;
	FMOD::System_Create(&system);
	FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_LOG);
	system->init(128, FMOD_INIT_NORMAL, nullptr);
	FMOD_VECTOR listenerPos = { earPos.x, earPos.y, earPos.z };
	FMOD_VECTOR listenerVel = { 0.0f, 0.0f, 0.0f };
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, nullptr, nullptr);
}

BreaksCore::~BreaksCore()
{
	//Release all sounds from the engine
	for (auto& allSounds : sounds) {
		allSounds.second->release();
	}
	sounds.clear();
	system->release();
	system = nullptr;
}

void BreaksCore::SetEarPos(Vector3& pos, bool isRelative, Vector3 forward, Vector3 up)
{
	FMOD_VECTOR listenerPos;
	FMOD_VECTOR listenerVel;
	if (isRelative) {
		listenerPos = { earPos.x + pos.x,
					   earPos.y + pos.y,
					   earPos.z + pos.z };
		listenerVel = { 0.0f, 0.0f, 0.0f };
	}
	else {
		listenerPos = { pos.x, pos.y, pos.z };
		listenerVel = { 0.0f, 0.0f, 0.0f };
	}
	earPos = { listenerPos.x, listenerPos.y, listenerPos.z };
	FMOD_VECTOR fvecForward{};
	fvecForward.x = forward.x;
	fvecForward.y = forward.y;
	fvecForward.z = forward.z;

	FMOD_VECTOR fvecUp{};
	fvecUp.x = up.x;
	fvecUp.y = up.y;
	fvecUp.z = up.z;

	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, &fvecForward, &fvecUp);
}

//The core update function
void BreaksCore::Update(float deltaTime)
{
	


	std::vector<BreaksChannelMap::iterator> stoppedChannels;
	//Itterate through each channel and call the channel specific update function via the state machine
	for (auto iterator = channelMap.begin(), end = channelMap.end(); iterator != end; iterator++) {
		iterator->second->Update(deltaTime);
		//If the channels state is set to STOPPED, push the channel to stopped channels
		if (iterator->second->state == BreaksChannel::State::STOPPED)
			stoppedChannels.push_back(iterator);
	}
	//Delete stopped channels
	for (auto& iterator : stoppedChannels) {
		channelMap.erase(iterator);
	}

	//Call FMOD system update
	system->update();
}

bool BreaksCore::CheckLoaded(int soundID)
{
	auto exists = sounds.find(soundID);
	return !(exists == sounds.end());
}

void BreaksCore::LoadSound(int soundID)
{
	//Ensure sound is registered
	auto data = soundDataMap.find(soundID);
	if (data == soundDataMap.end())
		return;

	//Generate FMOD MODE based on the parameters passed in, applying those params to the sound 
	FMOD_MODE mode = FMOD_NONBLOCKING;

	mode |= data->second->is3D ? FMOD_3D : FMOD_2D;
	mode |= data->second->isLoop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF;
	mode |= data->second->isStream ? FMOD_CREATESTREAM : FMOD_CREATECOMPRESSEDSAMPLE;

	//Create and register the sound
	FMOD::Sound* sound = nullptr;
	system->createSound(data->second->fileName.c_str(), mode, nullptr, &sound);
	if (sound) {
		sounds[soundID] = sound;
	}
}

