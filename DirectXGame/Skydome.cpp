#include "Skydome.h"
#include "assert.h"

void Skydome::Initialize(Model* model, ViewProjection* viewProjection) {
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// textureHandle_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	worldTransform_.scale_ = {100.0f, 100.0f, 100.0f};

	// 引数の内容をメンバ変数に記録
	viewProjection_ = viewProjection;
}

void Skydome::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

void Skydome::Draw() {
	// 3Dモデル描画
	model_->Draw(worldTransform_, *viewProjection_);
}
