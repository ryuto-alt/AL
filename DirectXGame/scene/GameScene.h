#pragma once
#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>

class GameScene {
public:
	GameScene();
	~GameScene();
	void Initialize();
	void Update();
	void Draw();

private:
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	uint32_t textureHandle_ = 0;
	uint32_t soundDataHandle_ = 0;
	Sprite* sprite_ = nullptr;
	std::vector<float> audioData_;
	float spriteSpeed_ = 5.0f;
	const int sampleRate_ = 44100;
	const int fftSize_ = 1024;
	const float frequencyThreshold_ = 1000.0f;
	const float energyThreshold_ = 10.0f;
};

struct WaveHeader {
	char chunkId[4];
	uint32_t chunkSize;
	char format[4];
	char subchunk1Id[4];
	uint32_t subchunk1Size;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;
	char subchunk2Id[4];
	uint32_t subchunk2Size;
};