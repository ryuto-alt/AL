#include "GameScene.h"
#include "Matrix.h"
#include "Matrix4x4.h"
#include "TextureManager.h"
#include "Vector3.h"
#include <cassert>
GameScene::GameScene() {}

GameScene::~GameScene() {
	delete modelBlock_;
	delete player;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
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

	textureHandle = TextureManager::Load("mario.jpg");

	modelBlock_ = Model::Create();

	worldTransform.Initialize();
	viewProjection.Initialize();

	// 自キャラの作成
	player = new Player();

	// 自キャラの初期化
	player->Initialize(modelBlock_, textureHandle, &viewProjection);

	debugCamera_ = new DebugCamera(1280, 720);
	// 要素数
	const uint32_t kNumBlockVirtical = 10;
	const uint32_t kNumBlockHorizontal = 20;

	// ブロック一個分の横幅
	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	// 要素数を変更する
	worldTransformBlocks_.resize(kNumBlockVirtical);

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			// Create holes by skipping every other block
			if ((i + j) % 2 == 0) {
				worldTransformBlocks_[i][j] = new WorldTransform();
				worldTransformBlocks_[i][j]->Initialize();
				worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;
				worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
			} else {
				// Set the block to nullptr to indicate a hole
				worldTransformBlocks_[i][j] = nullptr;
			}
		}
	}
}

void GameScene::Update() {

	// 自キャラの更新
	player->Update();

	Vector3 scale = {1.2f, 0.79f, -2.1f};
	Vector3 rotate = {0.4f, 1.43f, -0.8f};
	Vector3 translate = {2.7f, -4.15f, 1.57f};

	debugCamera_->Update();

#ifdef _DEBUG

	if (input_->TriggerKey(DIK_F1)) {
		isDebugCameraActive_ = true;

	}
		if (isDebugCameraActive_) {

			viewProjection.matView = debugCamera_->GetViewProjection().matView; // デバッグカメラのビュー行列
			viewProjection.matProjection = debugCamera_->GetViewProjection().matProjection;   // デバッグカメラのプロジェクション行列
		
			viewProjection.TransferMatrix();
		} else {
		
		
		viewProjection.UpdateMatrix();
		}
	

#endif // DEBUG

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
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
	// 自キャラの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock)
				continue;

			modelBlock_->Draw(*worldTransformBlock, viewProjection);
		}
	}
	/*player->Draw();*/
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
