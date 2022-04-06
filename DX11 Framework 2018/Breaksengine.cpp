#pragma once

#include "BreaksEngine.h"
#include "BreaksCore.h"
#include "BreaksChannel.h"

BreaksCore* core = nullptr;


BreaksEngine::BreaksEngine()
{
}

// ====== Initialise the BreaksCore Framework ====== //
void BreaksEngine::Initialize()
{
	core = new BreaksCore();
}

void BreaksEngine::Update(float elapsed)
{
	core->Update(elapsed);
}

void BreaksEngine::SetEarPos(Vector3 pos, bool isRelative, Vector3 forward, Vector3 up)
{
	core->SetEarPos(pos, isRelative, forward, up);
	
}

void BreaksEngine::GetOcclusion(FMOD_VECTOR* listenerPos, FMOD_VECTOR* sourcePos, float directOcclusion, float reverbOcclusion) {
	core->system->getGeometryOcclusion(listenerPos, sourcePos, &directOcclusion, &reverbOcclusion);
}
// ====== Shut down the Engine ====== //
void BreaksEngine::ShutDown()
{
	delete core;
}

//Registers a sound to a unique ID, giving the option to load it directly after
int BreaksEngine::RegisterSound(SoundData &soundData, bool load)
{
	int soundID = core->nextSoundID;
	core->nextSoundID++;
	core->soundDataMap[soundID] = &soundData;

	if (load) {
		core->LoadSound(soundID);
	}
	return soundID;
}

int BreaksEngine::PlayAudio(int soundID, Vector3 pos, float volume)
{
	int breaksChannelID = core->nextBreakChannelID;
	core->nextBreakChannelID++;
	auto found = core->sounds.find(soundID);

	//If the audio is not loaded, fail the function
	if (found == core->sounds.end())
	{
		return breaksChannelID;
	}
	core->channelMap[breaksChannelID] = std::make_unique<BreaksChannel>(*core, soundID, core->soundDataMap[soundID], VirtualSetting::MUTE, pos, volume);
	
	
	return breaksChannelID;
}



void BreaksEngine::LoadSound(int soundID)
{
	core->LoadSound(soundID);
}


void BreaksEngine::UnloadSound(int soundID)
{
	//If the sound is loaded, unload it. If it isnt then do nothing
	auto exists = core->sounds.find(soundID);
	if (exists != core->sounds.end()) {
		//Unload the sound
		exists->second->release();
		core->sounds.erase(exists);
	}
}


bool BreaksEngine::CheckLoaded(int soundID)
{
	return core->CheckLoaded(soundID);
}

// =============================== Channel Settings =============================== //
void BreaksEngine::SetBreaksChannelVolume(int channelID, float volume)
{
	auto exists = core->channelMap.find(channelID);
	if (exists != core->channelMap.end()) {
		exists->second->volume = volume;
		exists->second->channel->setVolume(volume);
	}
}

void BreaksEngine::SetSoundDirection(int channelID, Vector3 direction, Vector3 coneSettings) {

	FMOD_VECTOR coneDirection = { direction.x, direction.y, direction.z };
	FMOD_VECTOR settings = { coneSettings.x, coneSettings.y, coneSettings.z };

	auto exists = core->channelMap.find(channelID);
	if (exists != core->channelMap.end()) {
		exists->second->channel->set3DConeOrientation(&coneDirection);
											//inside angle, outside angle, outsidevolume
		exists->second->channel->set3DConeSettings(settings.x, settings.y, settings.z);
	}
}

void BreaksEngine::SetBreaksChannelPosition(int channelID, Vector3 pos, bool isRelative)
{
	auto exists = core->channelMap.find(channelID);
	if (exists != core->channelMap.end())
	{
		auto &channel = exists->second;
		channel->SetUpdateFlag(BreaksChannel::UpdateFlag::POSITION, true);
		if (isRelative) {
			Vector3 newPos = { channel->position.x + pos.x,
							   channel->position.y + pos.y,
							   channel->position.z + pos.z };
			channel->position = newPos;
		}
		channel->position = pos;
	}
}


void BreaksEngine::StopBreaksChannel(int channelID)
{
	auto exists = core->channelMap.find(channelID);
	if (exists != core->channelMap.end()) {
		exists->second->stopRequested = true;
	}
}

void BreaksEngine::VirtualiseBreaksChannel(int channelID)
{
	auto exists = core->channelMap.find(channelID);
	if (exists != core->channelMap.end()); {
		exists->second->virtualFlag = true;
	}
}

void BreaksEngine::DeVirtualiseBreaksChannel(int channelID)
{
	auto exists = core->channelMap.find(channelID);
	if (exists != core->channelMap.end()); {
		exists->second->virtualFlag = true;
	}

	
}

FMOD::Reverb3D* BreaksEngine::CreateReverb(Vector3 position, FMOD_REVERB_PROPERTIES properties) {
	FMOD_VECTOR pos = { position.x, position.y, position.z };

	FMOD::Reverb3D* reverb;
	core->system->createReverb3D(&reverb);
	reverb->setProperties(&properties);
	reverb->set3DAttributes(&pos, 30.0f, 150.0f);
	return reverb;
}

void BreaksEngine::CreateFmodGeometry(FMOD::Geometry* geometry, int maxPoligons, int maxVertices)
{
	core->system->createGeometry(maxPoligons, maxVertices, &geometry);
}
