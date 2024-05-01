#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include<fftw3.h>


// 周波数解析関数
std::vector<double> AnalyzeFrequency(const std::vector<uint8_t>& audioData) {
	std::vector<double> freqAmplitudes;
	size_t N = audioData.size();

	// FFT入力データを準備
	fftw_complex* in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
	fftw_complex* out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

	for (size_t i = 0; i < N; ++i) {
		in[i][0] = audioData[i];
		in[i][1] = 0.0;
	}

	// FFTの計算
	fftw_plan plan = fftw_plan_dft_1d((int)N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	// 周波数成分の振幅を取得
	for (size_t i = 0; i < N / 2; ++i) {
		double amplitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
		freqAmplitudes.push_back(amplitude);
	}

	fftw_destroy_plan(plan);
	fftw_free(in);
	fftw_free(out);

	return freqAmplitudes;
}

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	textureHandle_ = TextureManager::Load("mario.jpg");

	soundHandle_ = audio_->LoadWave("mario.mp3");
	//音楽再生
	audio_->PlayWave(soundHandle_);
	sprite_ = Sprite::Create(textureHandle_, {100, 50});
}

void GameScene::Update() {
	// 音楽データを取得
	const auto& audioData = audio_->GetAudioData(soundHandle_);

	// 周波数解析
	auto freqAmplitudes = AnalyzeFrequency(audioData);

	// 例えば、200Hzの成分に反応する
	size_t targetFreqIndex = 200;

	if (freqAmplitudes.size() > targetFreqIndex) {
		double amplitude = freqAmplitudes[targetFreqIndex];

		// 閾値チェック
		if (amplitude > 50.0) { // 例: 振幅50を閾値にする
			// 画像を右方向に移動
			Vector2 potision = sprite_->GetPosition();

			potision.x = 2.0f;
			potision.y = 1.0f;
		}
	}



}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	/// 
	sprite_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
