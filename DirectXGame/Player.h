#pragma once

#include <Model.h>
#include <WorldTransform.h>

// 左右
enum class LRDirection {
	kRight,
	kLeft,
};

/// <summary>
/// 自キャラ
/// </summary>
class Player {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model">モデル</param>
	/// <param name="textureHandle">テクスチャハンドル</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	WorldTransform* GetWorldTransform() { return &worldTransform_; }

	const Vector3& GetVelocity() { return velocity_; }

	float Lerp(float a, float b, float t) { return a + t * (b - a); }

private:
	// ワールド変換データ
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;
	// 速度
	Vector3 velocity_ = {};

	// 左右
	LRDirection lrDirection_ = LRDirection::kRight;

	// 接地状態フラグ
	bool onGround_ = true;

	// 加速度
	static inline const float kAcceleration = 0.025f;
	// 速度減衰率
	static inline const float kAttenuation = 0.05f;
	static inline const float kAttenuationLanding = 0.05f;
	// 最大速度
	static inline const float kLimitRunSpeed = 0.5f;
	// 旋回時間＜秒＞
	static inline const float kTimeTurn = 0.3f;
	// 重力加速度（下方向）
	static inline const float kGravityAcceleration = 0.25f;
	// 最大落下速度（下方向）
	static inline const float kLimitFallSpeed = 0.5f;
	// ジャンプ初速（上方向）
	static inline const float kJumpAcceleration = 1.5f;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
};
