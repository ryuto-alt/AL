#include "Enemy.h"
#include <iostream>

void Enemy::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, const Vector3& direction) {
	enemymodel_ = model;
	viewProjection_ = viewProjection;
	worldTransform_.translation_ = position;

	// 初期速度を設定
	velocity_ = direction;

	// ワールド変換データの初期化
	worldTransform_.Initialize();

	// エネミーを左向きに設定
	worldTransform_.rotation_.y = 3.14f / -2.0f; // -90度回転
}

void Enemy::Update() {
	// ①移動
	Move();

	// ワールド変換行列の更新
	worldTransform_.UpdateMatrix();
}

void Enemy::Draw() {
	if (enemymodel_ != nullptr && viewProjection_ != nullptr) {
		try {
			enemymodel_->Draw(worldTransform_, *viewProjection_);
		} catch (const std::exception& e) {
			std::cerr << "Exception caught in Enemy::Draw: " << e.what() << std::endl;
		} catch (...) {
			std::cerr << "Unknown exception caught in Enemy::Draw" << std::endl;
		}
	} else {
		std::cerr << "Model or ViewProjection is null" << std::endl;
	}
}

void Enemy::Move() {
	// ここでエネミーの移動ロジックを実装
	// 例: 速度に基づいて位置を更新
	worldTransform_.translation_ += velocity_;
}
