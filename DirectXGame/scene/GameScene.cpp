#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

#include "MatrixMath.h"

GameScene::GameScene() {}

GameScene::~GameScene() {
	// モデルの解放
	delete model_;
	delete modelBlock_;
	delete enemy_; // エネミーモデルの解放

	// 自キャラの解放
	delete player_;
	delete block_;
	// スカイドームの解放
	delete modelSkydome_;

	// デバッグカメラの解放
	delete debugCamera_;

	// マップチップフィールドの解放
	delete mapChipField_;

	// カメラコントローラの解放
	delete cameraController_;

	// worldTransformBlockの解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// エネミーの解放
	delete charaEnemy_; // エネミーの解放
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// スカイドームの3Dモデルデータの生成
	model_ = Model::CreateFromOBJ("player", true);
	enemy_ = Model::CreateFromOBJ("enemy", true);

	modelSkydome_ = Model::CreateFromOBJ("Skydome", true);
	modelBlock_ = Model::CreateFromOBJ("RoseCube", true);

	// カメラコントローラの生成
	cameraController_ = new CameraController();

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	// ビュープロジェクションの初期化
	viewProjection_.farZ = 2560;
	viewProjection_.translation_ = {0.0f, 0.0f, -20.0f};
	viewProjection_.Initialize();

	// スカイドームの生成
	skydome_ = new Skydome();

	// マップチップの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(1, 18);

	// 自キャラの生成
	player_ = new Player();
	// 自キャラの初期化
	player_->Initialize(model_, &viewProjection_, playerPosition);
	// マップチップデータのセット
	player_->SetMapChipField(mapChipField_);

	// スカイドームの初期化
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	// エネミーの生成と初期化
	charaEnemy_ = new Enemy();
	// プレイヤーの位置と同じに設定し、少し右に移動させる
	Vector3 enemyPosition = playerPosition + Vector3(6.0f, 0.0f, 0.0f);
	Vector3 enemyDirection = Vector3(-0.05f, 0.0f, 0.0f); // 左方向に移動
	charaEnemy_->Initialize(enemy_, &viewProjection_, enemyPosition, enemyDirection);

	// 表示ブロックの生成
	GenerateBlocks();

	// カメラコントローラの初期化
	cameraController_->Initialize(&viewProjection_);
	// 追従対象をセット
	cameraController_->SetTarget(player_);
	// リセット（瞬間合わせ）
	cameraController_->Reset();
}

void GameScene::Update() {
	// 自キャラの更新
	player_->Update();

	// エネミーの更新
	charaEnemy_->Update();

	// スカイドームの更新
	skydome_->Update();

	// カメラコントローラの更新
	cameraController_->Update();

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}

			// スケーリング行列の作成
			Matrix4x4 scale = MakeScaleMatrix(worldTransformBlock->scale_);

			// X軸周りの回転行列の作成
			Matrix4x4 rotateX = MakeRotateXMatrix(worldTransformBlock->rotation_.x);

			// Y軸周りの回転行列の作成
			Matrix4x4 rotateY = MakeRotateYMatrix(worldTransformBlock->rotation_.y);

			// Z軸周りの回転行列の作成
			Matrix4x4 rotateZ = MakeRotateZMatrix(worldTransformBlock->rotation_.z);

			// 回転行列の合成（X回転 * Y回転 * Z回転）
			Matrix4x4 rotationZXY = Multiply(rotateZ, Multiply(rotateX, rotateY));

			// 平行移動行列の作成
			Matrix4x4 translate = MakeTranslateMatrix(worldTransformBlock->translation_);

			// worldTransformBlocks->matWorld_ = スケーリング行列 * 回転行列 * 平行移動行列
			worldTransformBlock->matWorld_ = Multiply(scale, Multiply(rotationZXY, translate));
			worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
			// 定数のバッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

#ifdef _DEBUG
	// デバッグ開始
	if (input_->TriggerKey(DIK_TAB)) {
		// デバッグカメラ有効フラグをトグル
		isDebugCameraActive_ = !isDebugCameraActive_;
	}

	// カメラの処理
	if (isDebugCameraActive_) {
		// デバッグカメラの更新
		debugCamera_->Update();

		viewProjection_.matView = debugCamera_->GetViewProjection().matView;             // デバッグカメラのビュー行列
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection; // デバッグカメラのプロジェクション行列

		//  ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	} else {
		// ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}
#endif // _DEBUG
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
	// エネミーの描画
	charaEnemy_->Draw();
	// 自キャラの描画
	player_->Draw();

	// スカイドームの描画
	// skydome_->Draw();

	// ブロックの描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			if (!worldTransformBlock) {
				continue;
			}
			modelBlock_->Draw(*worldTransformBlock, viewProjection_);
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
		// １列要素数を設定（横方向のブロック数）
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	// ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}
