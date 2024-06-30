#pragma once
#include "Player.h"
#include "VectorMath.h"
#include <Model.h>
#include <WorldTransform.h>

class Enemy {
public:
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position, const Vector3& direction);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ①移動
	/// </summary>
	void Move();

	bool CheckCollisionWithPlayer(Player* player); // プレイヤーとの衝突判定メソッド

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* enemymodel_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	// 速度
	Vector3 velocity_ = {};
};
