#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "MapChipField.h" // 追加
#include "Model.h"
#include "Player.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <vector>

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

	void GenerateBlocks();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	uint32_t textureHandle = 0;
	Model* modelBlock_ = nullptr;

	// ワールドトランスフォーム
	WorldTransform worldTransform;
	// ビュープロジェクション
	ViewProjection viewProjection;
	std::vector<std::vector<WorldTransform*>> worldTransformBlocks_;
	Player* player = nullptr;

	bool isDebugCameraActive_ = true;
	DebugCamera* debugCamera_ = nullptr;
	MapChipField* mapChipField_; // 追加

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};
