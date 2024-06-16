#pragma once

#include "Vector3.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

// 前方宣言
class Player;

/// <summary>
/// カメラコントローラ
/// </summary>
class CameraController {
	// 矩形
	struct Rect {
		float left = 0.0f;  // 左端
		float right = 1.0f; // 右端
		float bottm = 0.0f; // 下端
		float top = 1.0f;   // 上端
	};

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(ViewProjection* viewProjection);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	// 外部からポインタをセットするためのsetterを用意
	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	// カメラ移動範囲
	void SetMovableArea(Rect area) { movableArea_ = area; }

	float Lerp(float a, float b, float t) { return a + t * (b - a); }

private:
	// 座標補間割合0.0～1.0までの実数値
	static inline const float kInterpolationRate = 0.1f;
	// 速度掛け率
	static inline const float kVelocityBias = 50.0f;
	// 追従対象の各方向へのカメラ移動範囲
	static inline const Rect margin = {-12,12, -20, 10};

	// ワールド変換データ
	WorldTransform worldTransform_;
	// ビュープロジェクション
	ViewProjection* viewProjection_ = nullptr;

	// 追従対象のポインタ
	Player* target_ = nullptr;

	// 追従対象とカメラの座標の差（オフセット）
	Vector3 targetOffset_ = {0.0f, 0.0f, -15.0f};

	// カメラ移動範囲
	Rect movableArea_ = {15, 100, 8, 100};

	// カメラの目標座標
	Vector3 goalPosition_;
};
