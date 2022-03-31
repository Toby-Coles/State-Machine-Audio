#include "BreaksChannel.h"
#include "BreaksEngine.h"
#include "BreaksCore.h"

// ============================================================================================================ // 
// ==================================== BreaksChannel Function Definitions==== ================================ // 
// ============================================================================================================ // 
BreaksChannel::BreaksChannel(BreaksCore& breaksCore, int soundID, SoundData* soundData, VirtualSetting virtSetting, Vector3& pos, float volume) :
	core(breaksCore), soundID(soundID), soundData(soundData), virtualSetting(virtSetting), virtualDistance(soundData->virtualDistance), position(pos) {}

BreaksChannel::~BreaksChannel()
{
}

void BreaksChannel::SetUpdateFlag(UpdateFlag updateFlag, bool flag)
{
	updateFlags.set(static_cast<size_t>(updateFlag), flag);
}

void BreaksChannel::Update(float deltaTime)
{
	virtualTimer += deltaTime;
	auto exists = core.sounds.find(soundID);;

	// State machine 
	switch (state)
	{
	case BreaksChannel::State::INIT:
		//[[fallthrough]];
		state = State::TOPLAY;
		break;
	case BreaksChannel::State::TOPLAY:
		// === TOPLAY ---> STOPPING === //
		// ============================ //
		if (stopRequested) {
			state = State::STOPPING;
			return;
		}
		// ============================ //

		// === TOPLAY ---> STOPPING/VIRTUAL === //
		// If the sound is a one shot, stop the sound
		// Otherwise virtualise the sound
		// ==================================== //
		if (VirtualCheck(true, deltaTime))
		{
			if (IsOneShot()) {
				state = State::STOPPING;
			}
			else
				state = State::VIRTUAL;
			return;
		}
		// =================================== //

		// === TOPLAY ---> LOADING === //
		// If the sound isnt loaded and needs to be 
		// =========================== //
		if (!core.CheckLoaded(soundID)) {
			core.LoadSound(soundID);
			state = State::LOADING;
			return;
		}
		// =========================== //

		// === TOPLAY ---> PLAYING === //
		// If there are no special cases,
		// continue and play the sound
		// =========================== //
		channel = nullptr;
		exists = core.sounds.find(soundID);
		if (exists != core.sounds.end()) {
			core.system->playSound(exists->second, nullptr, true, &channel);
		}
		if (channel) {
			state = State::PLAYING;
			FMOD_VECTOR pos{ position.x, position.y, position.z };
			channel->set3DAttributes(&pos, nullptr);
			channel->set3DMinMaxDistance(soundData->minDistance, soundData->maxDistance);
			channel->setVolume(volume);
			
			channel->setPaused(false);
		}
		else
			state = State::STOPPING;
		// =========================== //

		break;
	case BreaksChannel::State::PLAYING:
		UpdateParams();

		// === PLAYING ---> STOPPING === //
		// Does the sound need to be stopped?
		// =========================== //
		if (/*!IsPlaying() ||*/ stopRequested) {
			state = State::STOPPING;
		}
		// =========================== //

		// === PLAYING ---> VIRTUALISING === //
		// ================================= //
		if (virtualTimer < virtualCheckPlayPeriod) {
			if (VirtualCheck(false, deltaTime)) {
				state = State::VIRTUALISING;
			}
		}
		// ================================= //

		break;
	case BreaksChannel::State::LOADING:
		// === LOADING ---> TOPLAY === //
		//Check if the sound is loaded, if it is play it
		// =========================== //
		if (core.CheckLoaded(soundID)) {
			state = State::TOPLAY;
		}
		// =========================== //

		break;
	case BreaksChannel::State::PREPLAYING:
		// This state is active when the sound needs to be preplayed to
		// run the fade in effect.
		RunFadeIn(deltaTime);

		break;
	case BreaksChannel::State::VIRTUALISING:

		RunFadeOut(deltaTime);
		UpdateParams();
		if (!VirtualCheck(false, deltaTime)) {
			state = State::PREPLAYING;
		}

		break;
	case BreaksChannel::State::VIRTUAL:

		if (stopRequested) {
			state = State::STOPPING;
		}
		else if (!VirtualCheck(false, deltaTime)) {
			if (virtualSetting == VirtualSetting::RESTART) {
				state = State::INIT;
			}
			else if (virtualSetting == VirtualSetting::PAUSE) {
				channel->setPaused(false);
				state = State::PREPLAYING;
			}
			else {
				state = State::PREPLAYING;
			}
		}
		break;
	case BreaksChannel::State::STOPPING:
		//Run fade out logic and stop the channel
		RunFadeOut(deltaTime);
		UpdateParams();
		if (stopRequested || volume == 0.0f) {
			channel->stop();
			state = State::STOPPED;
			return;
		}
		break;
	case BreaksChannel::State::STOPPED:
		break;
	default:
		break;
	}
}

void BreaksChannel::UpdateParams()
{
	//Update virtual flags and parameters 
	if (updateFlags.test(static_cast<size_t>(UpdateFlag::POSITION))) {
		FMOD_VECTOR newPosition = { position.x, position.y, position.z };
		channel->set3DAttributes(&newPosition, nullptr);
		SetUpdateFlag(UpdateFlag::POSITION, false);
	}
}

//Run the fade in logic for a channel
void BreaksChannel::RunFadeIn(float deltaTime)
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

void BreaksChannel::RunFadeOut(float deltaTime)
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

bool BreaksChannel::VirtualCheck(bool allowOneShot, float deltaTime)
{
	if (virtualTimer < virtualCheckVirtualPeriod) {
		return (state == State::VIRTUALISING || state == State::VIRTUAL);
	}
	else if (isVirtFlagEffective) {
		return virtFlag;
	}
	else {
		virtualTimer = 0.0f;
		Vector3& earPosition = core.earPos;
		float deltaX = position.x - earPosition.x;
		float deltaY = position.y - earPosition.y;
		float deltaZ = position.z - earPosition.z;
		float distanceSquared = deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ;
		return(distanceSquared > virtualDistance * virtualDistance);
	}
}

bool BreaksChannel::IsPlaying()
{
	bool isPlaying = channel->isPlaying(&isPlaying);

	return isPlaying;
}

bool BreaksChannel::IsOneShot()
{

	return false;
}