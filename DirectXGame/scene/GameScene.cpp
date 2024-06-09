#include "GameScene.h"
#include "TextureManager.h"
#include <cassert>

// x軸回転行列
Matrix4x4 GameScene::MakeRotateXMatrix(float radian) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = 1;
	ans.m[1][1] = std::cos(radian);
	ans.m[1][2] = std::sin(radian);
	ans.m[2][1] = -std::sin(radian);
	ans.m[2][2] = std::cos(radian);
	ans.m[3][3] = 1;

	return ans;
};

// y軸回転行列
Matrix4x4 GameScene::MakeRotateYMatrix(float radian) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = std::cos(radian);
	ans.m[0][2] = -std::sin(radian);
	ans.m[1][1] = 1;
	ans.m[2][0] = std::sin(radian);
	ans.m[2][2] = std::cos(radian);
	ans.m[3][3] = 1;

	return ans;
};

// z軸回転行列
Matrix4x4 GameScene::MakeRotateZMatrix(float radian) {
	Matrix4x4 ans = { 0 };

	ans.m[0][0] = std::cos(radian);
	ans.m[0][1] = std::sin(radian);
	ans.m[1][0] = -std::sin(radian);
	ans.m[1][1] = std::cos(radian);
	ans.m[2][2] = 1;
	ans.m[3][3] = 1;

	return ans;
};

Matrix4x4 GameScene::Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 ans;
	for (int a = 0; a < 4; a++) {
		for (int b = 0; b < 4; b++) {
			ans.m[a][b] = m1.m[a][0] * m2.m[0][b] + m1.m[a][1] * m2.m[1][b] + m1.m[a][2] * m2.m[2][b] + m1.m[a][3] * m2.m[3][b];
		}
	}
	return ans;
};

// ３次元アフィン変換行列
Matrix4x4 GameScene::MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 S = { 0 };
	Matrix4x4 R = { 0 };
	Matrix4x4 rX = { 0 };
	Matrix4x4 rY = { 0 };
	Matrix4x4 rZ = { 0 };
	Matrix4x4 T = { 0 };
	Matrix4x4 ans = { 0 };

	S.m[0][0] = scale.x;
	S.m[1][1] = scale.y;
	S.m[2][2] = scale.z;
	S.m[3][3] = 1;

	rX = MakeRotateXMatrix(rotate.x);
	rY = MakeRotateYMatrix(rotate.y);
	rZ = MakeRotateZMatrix(rotate.z);

	R = Multiply(rX, Multiply(rY, rZ));

	T.m[0][0] = 1;
	T.m[1][1] = 1;
	T.m[2][2] = 1;
	T.m[3][3] = 1;
	T.m[3][0] = translate.x;
	T.m[3][1] = translate.y;
	T.m[3][2] = translate.z;

	ans.m[0][0] = S.m[0][0] * R.m[0][0];
	ans.m[0][1] = S.m[0][0] * R.m[0][1];
	ans.m[0][2] = S.m[0][0] * R.m[0][2];
	ans.m[1][0] = S.m[1][1] * R.m[1][0];
	ans.m[1][1] = S.m[1][1] * R.m[1][1];
	ans.m[1][2] = S.m[1][1] * R.m[1][2];
	ans.m[2][0] = S.m[2][2] * R.m[2][0];
	ans.m[2][1] = S.m[2][2] * R.m[2][1];
	ans.m[2][2] = S.m[2][2] * R.m[2][2];
	ans.m[3][0] = T.m[3][0];
	ans.m[3][1] = T.m[3][1];
	ans.m[3][2] = T.m[3][2];
	ans.m[3][3] = 1;

	return ans;
};

GameScene::GameScene() {}

GameScene::~GameScene() {

	delete model_;
	delete player_;
	delete modelBlock_;
	delete debugCamera_;
	delete skydome_;
	delete mapChipField_;

	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("player.png");

	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// Player
	player_ = new Player();
	model_ = Model::CreateFromOBJ("player", true);
	Vector3 playerPosition = mapChipField_->GetMapChipPostionByIndex(5, 18);
	player_->Initialize(model_, &viewProjection_, playerPosition);

	// skydome
	skydome_ = new Skydome();
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_->Initialize(modelSkydome_, &viewProjection_);

	// マップチップ
	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/map.csv");
	GenerateBlocks();

	// ブロック
	modelBlock_ = Model::Create();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);
}


void GameScene::GenerateBlocks() {
	// 要素数
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();     // 縦
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal(); // 横

	// 要素数を変更する
	// 列数を設定(縦方向のブロック数)
	worldTransformBlocks_.resize(numBlockVirtical);
	for (uint32_t i = 0; i < numBlockVirtical; i++) {
		//1列の要素数を設定(横方向のブロック数)
		worldTransformBlocks_[i].resize(numBlockHorizontal);
	}

	//ブロックの生成
	for (uint32_t i = 0; i < numBlockVirtical; i++) {
		for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				WorldTransform* worldTransform = new WorldTransform();
				worldTransform->Initialize();
				worldTransformBlocks_[i][j] = worldTransform;
				worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChipPostionByIndex(j, i);
			}
		}
	}
}


void GameScene::Update() {

#ifdef _DEBUG
	if (input_->TriggerKey(DIK_SPACE)) {
		if (isDebugCameraActive_ == true)
			isDebugCameraActive_ = false;
		else
			isDebugCameraActive_ = true;
	}
#endif

	if (isDebugCameraActive_) {
		debugCamera_->Update();
		viewProjection_.matView = debugCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = debugCamera_->GetViewProjection().matProjection;
		//ビュープロジェクション行列の転送
		viewProjection_.TransferMatrix();
	}
	else
	{
		//ビュープロジェクション行列の更新と転送
		viewProjection_.UpdateMatrix();
	}

	// ブロックの更新
	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			if (!worldTransformBlock)
				continue;
			// アフィン変換行列の作成
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	// 自キャラの更新
	player_->Update();

	// 天球の更新
	skydome_->Update();
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
	// ブロックの描画
	
	// 自キャラの描画
	player_->Draw();
	// 天球の描画
	/*skydome_->Draw();*/

	for (std::vector<WorldTransform*>& worldtransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldtransformBlockLine) {
			if (!worldTransformBlock)
				continue;
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