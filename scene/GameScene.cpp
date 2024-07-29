#include "GameScene.h"
#include "TextureManager.h"
#include "myMath.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete model_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}

	worldTransformBlocks_.clear();

	delete debugCamera_;

	delete modelSkydome_;

	delete mapChipField_;

	delete modelPlayer_;

	delete modelEnemy_;

	delete deathParticles_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();

	// ファイル名を指定してテクスチャを読み込む
	//textureHandle_ = TextureManager::Load("uvChecker.png");
	// 3Dモデルの生成
	model_ = Model::Create();
	modelBlock_ = Model::CreateFromOBJ("block", true);

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(5, 18);
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(9, 18);

	// マップチップフィールドの生成
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");

	// 自キャラの生成
	player_ = new Player();
	modelPlayer_ = Model::CreateFromOBJ("Player",true);

	// 自キャラの初期化
	player_->Initialize(modelPlayer_,&viewProjection_,playerPosition);
	player_->SetMapChipField(mapChipField_);


	// 天球の生成
	skydome_ = new Skydome();
	// 天球3Dモデルの生成
	modelSkydome_ = Model::CreateFromOBJ("sphere", true);
	// 天球の初期化
	skydome_->Initialize(modelSkydome_,&viewProjection_);

	GenerateBlcoks();

	// カメラコントロールの初期化
	cameraController_ = new CameraController();// 生成
	cameraController_->Initialize();
	cameraController_->SetTarget(player_);
	cameraController_->Reset();
	CameraController::Rect movableArea_ = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(movableArea_);

	for (int32_t i= 0; i < 3; ++i) {
		Enemy*newEnemy = new Enemy();
		std::vector<Vector3> enemyPositions = {
		{8.0f, 2.0f, 0.0f},
		{8.0f, 4.0f, 0.0f},
		{8.0f, 6.0f, 0.0f}
		};
		modelEnemy_ = Model::CreateFromOBJ("enemy", true);
		newEnemy->Initialize(modelEnemy_, &viewProjection_, enemyPositions[i]);
		enemies_.push_back(newEnemy);
	}

	modelDeathParticlse_ = Model::CreateFromOBJ("deathParticle", true);

	deathParticles_ = new DeathParticles();
	deathParticles_->Initialize(modelDeathParticlse_, &viewProjection_, playerPosition);


	phase_ = Phase::kPlay;

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280,720);
}

void GameScene::Update() {

	ChangePhase();

	switch (phase_){
	case Phase::kPlay:

		// 自キャラの更新
		player_->Update();

		// 天球の更新
		skydome_->Update();

		// カメラコントローラの更新
		cameraController_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// 縦横ブロック更新
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクションの転送
			viewProjection_.TransferMatrix();
		}

		#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugCameraActive_ == true)
			isDebugCameraActive_ = false;
		else
			isDebugCameraActive_ = true;
	}
#endif

		// 全てのあたり判定を行う
		CheckAllCollisions();
		break;
	case Phase::kDeath:

		// 天球の更新
		skydome_->Update();

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}

		// 縦横ブロック更新
		for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
			for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
				if (!worldTransformBlockYoko)
					continue;

				// アフィン変換行列の作成
				//(MakeAffineMatrix：自分で作った数学系関数)
				worldTransformBlockYoko->matWorld_ = 
					MakeAffineMatrix(worldTransformBlockYoko->scale_, worldTransformBlockYoko->rotation_, worldTransformBlockYoko->translation_);

				// 定数バッファに転送
				worldTransformBlockYoko->TransferMatrix();
			}
		}

		// カメラ処理
		if (isDebugCameraActive_) {
			// デバッグカメラの更新
			debugCamera_->Update();
			viewProjection_.matView = debugCamera_->GetViewProjection().matView;
			viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
			// ビュープロジェクション行列の転送
			viewProjection_.TransferMatrix();
		} else {
			// ビュープロジェクション行列の更新と転送
			viewProjection_.matView = cameraController_->GetViewProjection().matView;
			viewProjection_.matProjection = cameraController_->GetViewProjection().matProjection;
			// ビュープロジェクションの転送
			viewProjection_.TransferMatrix();
		}

		if (deathParticles_) {
			deathParticles_->Update();
		}
		break;
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
	

	// 天球の描画
	skydome_->Draw();

	// 敵の描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	//縦横ブロック描画
    for (std::vector<WorldTransform*> worldTransformBlockTate : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlockYoko : worldTransformBlockTate) {
			if (!worldTransformBlockYoko)
				continue;

			modelBlock_->Draw(*worldTransformBlockYoko, viewProjection_);
		}
	}

	if (!player_->IsDead()) {
		player_->Draw();
	}

	if (deathParticles_) {
		deathParticles_->Draw();
	}

	switch (phase_)
	{
	case GameScene::Phase::kPlay:
		// 自キャラの描画
		player_->Draw();
		break;
	case GameScene::Phase::kDeath:
		break;
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

void GameScene::GenerateBlcoks()
{
	uint32_t numBlockVirticle = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	// 要素数を変更する
	worldTransformBlocks_.resize(numBlockVirticle);

	// キューブの生成
	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	for (uint32_t i = 0; i < numBlockVirticle; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j,i)==MapChipType::lBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollisions()
{
	{
#pragma region 自キャラと敵キャラの当たり判定
	
		// 判定対象1と2の座標
		AABB aabb1, aabb2;
		

		// 自キャラの座標
		aabb1 = player_->GetAABB();
		for (Enemy* enemy : enemies_) {
			// 敵弾の座標
			aabb2 = enemy->GetAABB();

			// AABB同士の交差判定(
			if (IsCollision(aabb1, aabb2)) {
				// 自キャラの衝突判定コールバックを呼び出す
				player_->OnCollision(enemy);
				// 敵弾の衝突判定コールバックを呼び出す
					enemy->OnCollision(player_);
			}
		}
	}
#pragma endregion
}

void GameScene::ChangePhase()
{
	switch (phase_){
	case Phase::kPlay:

		// 自キャラの状態をチェック
		if (player_->IsDead()) {
			// 死亡フェーズに切り替え
			phase_ = Phase::kDeath;
			// 自キャラの座標を取得
			const Vector3& deathParticlesPosition = player_->GetWorldPosition();
			// 自キャラの座標にデスパーティクルを発生、初期化
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize(modelDeathParticlse_, &viewProjection_, deathParticlesPosition);
		}

		// 全てのあたり判定を行う
		CheckAllCollisions();

		break;
	case Phase::kDeath:
		// デス演出フェーズの処理
		if (deathParticles_ && deathParticles_->IsFinished()) {
			finished_=true;
		}
		break;
	}
}
