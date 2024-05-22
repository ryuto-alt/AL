#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model_, uint32_t textureHandle_, ViewProjection* viewProjection_) {

	/*assert(model);*/
	// メンバ変数にデータを記録
	model = model_;
	textureHandle = textureHandle_;

	worldTransform.Initialize();

	viewProjection = viewProjection_;

	debugCamera_ = new DebugCamera(100, 100);
}

void Player::Update() {

	worldTransform.TransferMatrix();

	debugCamera_->Update();
}

void Player::Draw() { model->Draw(worldTransform, debugCamera_->GetViewProjection(), textureHandle); }
