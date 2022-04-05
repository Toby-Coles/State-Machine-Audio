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

void BreaksEngine::SetEarPos(Vector3 pos, bool isRelative)
{
	core->SetEarPos(pos, isRelative);
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

void BreaksEngine::SetBreaksChannelVolume(int channelID, float volume)
{
	auto exists = core->channelMap.find(channelID);
	if (exists != core->channelMap.end()) {
		exists->second->volume = volume;
		exists->second->channel->setVolume(volume);
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
