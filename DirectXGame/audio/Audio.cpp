#include "Audio.h"
#include <cassert>
const std::vector<uint8_t>& Audio::XAudio2VoiceCallback::GetAudioData(uint32_t soundDataHandle) const {
	assert(soundDataHandle < kMaxSoundData);
	return soundDatas_[soundDataHandle].buffer;
}


