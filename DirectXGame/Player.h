#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "assert.h"

class Player {

	enum class LRDirection {
		kRight,
		kLeft,
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;

	// モデル
	Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	Vector3 velocity_ = {};
	static inline const float kAcceleration = 1;
	static inline const float kAttenuation = 1;
	static inline const float kLimitRunSpeed = 1;

	LRDirection lrDirection_ = LRDirection::kRight;

	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 接点状態フラグ
	bool onGround_ = true;

	// 重力加速度
	static inline const float kGravityAccleration = 0.1f;
	// 最大落下速度
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速
	static inline const float kJampAcceleration = 1.0f;
};