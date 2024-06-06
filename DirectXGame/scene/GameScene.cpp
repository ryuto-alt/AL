#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>
#include <Matrix.h>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;
	delete mapChipField_;
	delete player_;
	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();
	delete debugCamera_;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");
	// 3Dモデルの生成
	model_ = Model::Create();
	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, textureHandle_, &viewProjection_);


	debugCamera_ = new DebugCamera(1280, 720);
	// ブロックの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	GenerateBlocks();

}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();

	Vector3 scale = {1.2f, 0.79f, -2.1f};
	Vector3 rotate = {0.4f, 1.43f, -0.8f};
	Vector3 translate = {2.7f, -4.15f, 1.57f};

	debugCamera_->Update();

	#ifdef _DEBUG

	if (input_->TriggerKey(DIK_F1)) {
		isDebugCameraActive_ = true;
	}
	if (isDebugCameraActive_) {

		viewProjection_.matView = debugCamera_->GetViewProjection().matView;             // デバッグカメラのビュー行列
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection; // デバッグカメラのプロジェクション行列

		viewProjection_.TransferMatrix();
	} else {

		viewProjection_.UpdateMatrix();
	}

#endif // DEBUG


	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);

			worldTransformBlock->TransferMatrix();
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
	// 3Dモデル描画
	//	model_->Draw(worldTransform_, viewProjection_, textureHandle_);
	// 自キャラの描画
	/*player_->Draw();*/


	for (auto& worldTransformBlockLine : worldTransformBlocks_) {
		for (auto& worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			model_->Draw(*worldTransformBlock, viewProjection_);
		
		}
	}
	

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	// 列数を設定（縦方向のブロック数）
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		// 列の要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(numBlockHorizontal);

		// ブロックの生成
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) { // ループインデックスの重複を修正
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransformBlock = new WorldTransform(); // 名前を変更
				worldTransformBlock->Initialize();
				worldTransformBlocks_[i][j] = worldTransformBlock;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			} else {
				worldTransformBlocks_[i][j] = nullptr;
			}
		}
	}
}