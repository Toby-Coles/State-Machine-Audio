
#include "BreaksEngine.h"
#include <map>
#include <vector>
#include <iterator>
#include "Vector3.cpp"

// ========================== Breaks Core ========================== //
//Breaks core contains the core functionaliy of the engine, including
//the state machine architecture and FMOD::Channel wrapper used to pass
//sounds through the state machine structure.
struct BreaksCore {

	BreaksCore();
	~BreaksCore();
	void SetEarPos(Vector3& pos, bool isRelative);
	void Update(float deltaTime);
	bool CheckLoaded(int soundID);
	void LoadSound(int soundID);

	//Fmod system definition
	FMOD::System* system;

	//Defines the virtual settings a channel can use to select its virtualisation preference
	enum class VirtualSetting
	{
		RESTART, PAUSE, MUTE
	};

	// ================= FMOD Channell Wrapper ======================= //
	// == The BreaksChannel wraps the FMOD::Channel so that it     == //
	// == can be used as a part of the state machine, containing  == //
	// == its channel, sound data and unique ID to keep track of == //
	// == each channel as it is used through the state machine  == //

	struct BreaksChannel {
		BreaksChannel(BreaksCore& breaksCore, int soundID, BreaksEngine::SoundData* soundData, VirtualSetting virtSetting, Vector3& pos, float volume);
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

		//Channel Wrapper Data Variables
		BreaksCore& core;
		FMOD::Channel* channel = nullptr;
		BreaksEngine::SoundData* soundData;
		int soundID;

		
		//Virtualisation Settings
		VirtualSetting virtualSetting;

		bool virtualFlag = false;
		float virtualDistance;
		float virtualTime;
		float virtualCheckPlayPeriod;
		float virtualCheckVirtualPeriod;
		float virtualFadeInTime;
		float virtualFadeOutTime;
		float stopFadeOutTime;

		State state = State::INIT;
		float volume = 1.0f;
		bool stopRequested = false;
		Vector3 position;

		void SetUpdateFlag();
		void Update(float deltaTime);
		void UpdateParams();
		void RunFadeIn(float deltaTime);
		void RunFadeOut(float deltaTime);
		bool VirtualCheck(bool allowOneShot, float deltaTime);
		bool IsPlaying();
		bool IsOneShot();

	};

	Vector3 earPos;

	int nextSoundID;
	int nextBreakChannelID;

	//Define the maps for data to be stored in for the engine
	typedef std::map<int, FMOD::Sound*> SoundMap;
	typedef std::map<int, BreaksEngine::SoundData*> SoundDataMap;
	typedef std::map<int, std::unique_ptr<BreaksChannel>> BreaksChannelMap;

	SoundMap sounds;
	SoundDataMap soundDataMap;
	BreaksChannelMap channelMap;
};

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
	for (auto allSounds : sounds) {
		allSounds.second->release();
	}
	sounds.clear();
	system->release();
	system = nullptr;
}

void BreaksCore::SetEarPos(Vector3& pos, bool isRelative)
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
	system->set3DListenerAttributes(0, &listenerPos, &listenerVel, nullptr, nullptr);
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
	system->createSound(data->second->name.c_str(), mode, nullptr, &sound);
	if (sound) {
		sounds[soundID] = sound;
	}
}

// ============================================================================================================ // 
// ==================================== BreaksChannel Function Definitions==== ================================ // 
// ============================================================================================================ // 


BreaksCore::BreaksChannel::BreaksChannel(BreaksCore& breaksCore, int soundID, BreaksEngine::SoundData* soundData, VirtualSetting virtSetting, Vector3& pos, float volume) : 
	core(breaksCore), soundID(soundID), soundData(soundData), virtualSetting(virtSetting), virtualDistance(soundData->virtualDistance), position(pos) {}




BreaksCore::BreaksChannel::~BreaksChannel()
{
}

void BreaksCore::BreaksChannel::SetUpdateFlag()
{
}

void BreaksCore::BreaksChannel::Update(float deltaTime)
{
	virtualTime += deltaTime;
	// State machine 
	switch (state)
	{
	case BreaksCore::BreaksChannel::State::INIT:
		[[fallthrough]];
		break;
	case BreaksCore::BreaksChannel::State::TOPLAY:
		// === TOPLAY ---> STOPPING === //
		// ========================= //
		if (stopRequested) {
			state = State::STOPPING;
			return;
		}
		// ========================= //


		// === TOPLAY ---> STOPPING/VIRTUAL === //
		// If the sound is a one shot, stop the sound
		// Otherwise virtualise the sound
		// ========================= //
		if (VirtualCheck(true, deltaTime))
		{
			if (IsOneShot()) {
				state = State::STOPPING;
			}
			else
				state = State::VIRTUAL;
			return;
		}
		// ========================= //





		break;
	case BreaksCore::BreaksChannel::State::PLAYING:
		break;
	case BreaksCore::BreaksChannel::State::LOADING:
		break;
	case BreaksCore::BreaksChannel::State::PREPLAYING:
		break;
	case BreaksCore::BreaksChannel::State::VIRTUALISING:
		break;
	case BreaksCore::BreaksChannel::State::VIRTUAL:
		break;
	case BreaksCore::BreaksChannel::State::STOPPING:
		break;
	case BreaksCore::BreaksChannel::State::STOPPED:
		break;
	default:
		break;
	}
}

void BreaksCore::BreaksChannel::UpdateParams()
{
}

//Run the fade in logic for a channel
void BreaksCore::BreaksChannel::RunFadeIn(float deltaTime)
{
	float currentVolume;
	channel->getVolume(&currentVolume);
	float newVolume = currentVolume + deltaTime / virtualFadeInTime;
	if (newVolume >= volume) {
		if (state == State::PREPLAYING) {
			state = State::PLAYING;
		}
	}
	else {
		channel->setVolume(newVolume);
	}
}

void BreaksCore::BreaksChannel::RunFadeOut(float deltaTime)
{
	float currentVolume;
	channel->getVolume(&currentVolume);
	float newVolume = currentVolume;
	if (state == State::STOPPING) {
		newVolume = currentVolume - deltaTime / stopFadeOutTime;
	}
	else if (state == State::VIRTUALISING) {
		newVolume = currentVolume / deltaTime / virtualFadeOutTime;
	}
	if (newVolume <= 0.0f) {
		if (state == State::STOPPING) {
			channel->stop();
			state = State::STOPPED;
		}
		else if (state == State::VIRTUALISING) {
			state = State::VIRTUAL;
			if (virtualSetting == VirtualSetting::RESTART) {
				channel->stop();
			}
			else if (virtualSetting == VirtualSetting::PAUSE) {
				channel->setPaused(true);
			}
			else if (virtualSetting == VirtualSetting::MUTE) {
				channel->setVolume(0.0f);
			}
		}
		channel->setVolume(newVolume);

	}
}

bool BreaksCore::BreaksChannel::VirtualCheck(bool allowOneShot, float deltaTime)
{
	return false;
}

bool BreaksCore::BreaksChannel::IsPlaying()
{
	bool isPlaying = channel->isPlaying(&isPlaying);

	return isPlaying;
}

bool BreaksCore::BreaksChannel::IsOneShot()
{

	return false;
}
