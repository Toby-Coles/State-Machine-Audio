
#include "BreaksEngine.h"
#include <map>

// ========================== Breaks Core ========================== //
//Breaks core contains the core functionaliy of the engine, including
//the state machine architecture and FMOD::Channel wrapper used to pass
//sounds through the state machine structure.
struct BreaksCore {

	void SetEarPos(Vec3& pos, bool isRelative);
	void Update(float deltaTime);
	bool CheckLoaded();
	void LoadSound(int soundID);

	//Fmod system definition
	FMOD::System* system;

	//Defines the virtual settings a channel can use to select its virtualisation preference
	enum class VirtualSetting
	{
		RESTART, PAUSE, MUTE
	};

	// ================= FMOD Channell Wrapper ================= //
	struct BreaksChannel {
		BreaksChannel(BreaksCore& core, int soundID, BreaksEngine::SoundData soundData, VirtualSetting virtSetting, Vec3& pos, float volume);
		~BreaksChannel();

		//Defines the different states of the state machine
		enum class State {
			INIT,
			TOPLAY,
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

		float virtualDistance;
		float virtualTime;
		float virtualCheckPlayPeriod;
		float virtualCheckVirtualPeriod;
		float virtualFadeInTime;
		float virtualFadeOutTime;

		State state = State::INIT;
		float volume = 1.0f;
		bool StopRequested = false;
		Vec3 position;

		void SetUpdateFlag();
		void Update();
		void UpdateParams();
		void SetFadeIn();
		void SetFadeOut();
		bool VirtualCheck();
		bool IsPlaying();
		bool IsOneShot();

	};

	Vec3 earPos;

	//Define the maps for data to be stored in for the engine
	typedef std::map<int, FMOD::Sound*> SoundMap;
	typedef std::map<int, BreaksEngine::SoundData*> SoundDataMap;
	typedef std::map<int, BreaksChannel> BreaksChannelMap;

	SoundMap sounds;
	SoundDataMap soundDataMap;
	BreaksChannelMap channelMap;
};