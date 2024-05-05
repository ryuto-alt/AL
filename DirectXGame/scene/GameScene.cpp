#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <cmath>
#include <fftw3.h>
#include <iostream>

GameScene::GameScene() {}

GameScene::~GameScene() { delete sprite_; }

void GameScene::Initialize() {
	dxCommon_ = DirectXComamon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandle_ = TextureManager::Load("mario.jpg");
	sprite_ = Sprite::Create(textureHandle_, {100.0f, 100.0f});
	soundDataHandle_ = audio_->LoadWave("mario.wav");
	audio_->PlayWave(soundDataHandle_);
	audioData_ = audio_->GetWaveData(soundDataHandle_);
}

void GameScene::Update() {
	// FFTの実行
	fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize_);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * fftSize_);
	fftw_plan plan = fftw_plan_dft_1d(fftSize_, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	for (int i = 0; i < fftSize_; ++i) {
		in[i][0] = (i < static_cast<int>(audioData_.size())) ? audioData_[i] : 0.0f;
		in[i][1] = 0.0f;
	}

	fftw_execute(plan);

	// 周波数帯域のエネルギーを計算
	float energy = 0.0f;
	for (int i = 0; i < fftSize_ / 2; ++i) {
		float frequency = static_cast<float>(i * sampleRate_) / fftSize_;
		if (frequency >= frequencyThreshold_) {
			float real = static_cast<float>(out[i][0]);
			float imag = static_cast<float>(out[i][1]);
			energy += std::sqrt(real * real + imag * imag);
		}
	}

	// エネルギーに基づいてスプライトを移動
	Vector2 position = sprite_->GetPosition();
	if (energy > energyThreshold_) {
		position.x += spriteSpeed_;
		if (position.x > 1280.0f) {
			position.x = 0.0f;
		}
	}
	sprite_->SetPosition(position);

	fftw_destroy_plan(plan);
	fftw_free(in);
	fftw_free(out);
}

void GameScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// スプライト描画
	sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
}