#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <cmath>
#include <complex>
#include <fftw3.h>
#include <fstream>
#include <vector>
#define PI 3.14
#include "ImGuiManager.h"
#include <iostream>

std::vector<float> LoadPCMData(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary);
	std::vector<float> pcmData;
	if (!file) {
		std::cerr << "Failed to open file: " << filename << std::endl;
		return pcmData;
	}

	// WAVヘッダーをスキップ
	file.ignore(44);

	// 16ビット整数としてPCMデータを読み込む
	int16_t sample;
	while (file.read(reinterpret_cast<char*>(&sample), sizeof(int16_t))) {
		// -1.0 から 1.0 の範囲に正規化
		pcmData.push_back(sample / 32768.0f);
	}

	file.close();
	return pcmData;
}

using namespace std;

const int SAMPLE_RATE = 44100; // サンプリング周波数
const int WINDOW_SIZE = 512;  // サンプリングウィンドウサイズ

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandle_ = TextureManager::Load("mario.jpg");
	sprite_ = Sprite::Create(textureHandle_, {100, 100});
	soundDataHandle_ = audio_->LoadWave("mario.wav");
	audio_->PlayWave(soundDataHandle_);
}

void GameScene::Update() {
	Vector2 position = sprite_->GetPosition();
	bool moveSprite = false;
	double high_freq_threshold = 10.0; // 500Hz
	double amplitude_threshold = 0.1;   // 振幅の閾値
	double maxAmplitude = 0.0;
	int maxIndex = 0;

	// FFTの実行
	fftw_complex *in, *out;
	fftw_plan p;
	const int N = WINDOW_SIZE;
	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);
	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	// PCMデータの読み込みとFFTの実行
	std::vector<float> pcmData = LoadPCMData("mario.wav");
	for (int i = 0; i < N && i < pcmData.size(); i++) {
		in[i][0] = pcmData[i];
		in[i][1] = 0.0;
	}


	fftw_execute(p);

	// FFT結果の解析
	for (int i = 0; i <= N / 2; i++) {
		double freq = i * SAMPLE_RATE / N;
		double amplitude = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
		if (amplitude > maxAmplitude) {
			maxAmplitude = amplitude;
			maxIndex = i;
		}
		if (freq > high_freq_threshold && amplitude > amplitude_threshold) {
			moveSprite = true;
		}
	}

	// デバッグ情報の出力
	ImGui::Begin("Debug Info");
	ImGui::Text("Max Amplitude: %.5f at %f Hz", maxAmplitude, maxIndex * SAMPLE_RATE / N);
	ImGui::Text("moveSprite: %s", moveSprite ? "true" : "false");
	ImGui::End();

	// スプライトの移動
	if (moveSprite) {
		position.x += 5.0f;
		sprite_->SetPosition(position);
	}

	fftw_free(in);
	fftw_free(out);
	fftw_destroy_plan(p);
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
	sprite_->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion

	
}