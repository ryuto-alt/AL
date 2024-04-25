#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	uint32_t textureHandle = 0;
	Sprite* sprite = nullptr;

	Model* model = nullptr;

	WorldTransform worldTransform;
	ViewProjection viewProjection;

	uint32_t soundDataHandle = 0;

	// 音声再生ハンドル
	uint32_t voiceHandle = 0;

	// ImGuiで値を入力する変数
	float inputFloat3[3] = {0, 0, 0};

	// デバッグカメラ
	DebugCamera* debugCamera = nullptr;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
