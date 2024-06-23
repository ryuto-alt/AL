#pragma once

#include "VectorMath.h"
#include <Model.h>
#include <WorldTransform.h>

// マップとの当たり判定情報
struct CollisionMapInfo {
	bool isCeiling = false; // 天井衝突フラグ
	bool isLanding = false; // 着地フラグ
	bool isWall = false;    // 壁接触フラグ
	Vector3 move;           // 移動量
};

// 前方宣言
class MapChipField;

// 左右
enum class LRDirection {
	kRight,
	kLeft,
};

// キャラクターの当たり判定サイズ
static inline const float kWidth = 0.8f;
static inline const float kHeight = 0.8f;

// プレイヤーがマップチップに表示される番号
static inline const float kBlank = 18.0f;

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

	/// <summary>
	/// ①移動
	/// </summary>
	void Move();

	/// <summary>
	/// ②マップ衝突判定
	/// </summary>
	void MapCollisionDetection(CollisionMapInfo& info);

	/// <summary>
	/// ②マップ↓衝突
	/// </summary>
	void BottomDetection(CollisionMapInfo& info);


	void GroundDetection(CollisionMapInfo& info);

	/// <summary>
	/// ③判定結果を反映して移動させる
	/// </summary>
	/// <param name="info"></param>
	void MoveReflectJudgmentResult(CollisionMapInfo& info);

	/// <summary>
	/// ④天井に接触している場合の処理
	/// </summary>
	/// <param name="info"></param>
	void TouchingCeiling(CollisionMapInfo& info);

	/// <summary>
	/// ⑦旋回制御
	/// </summary>
	void TurningControl();

	/// <summary>
	/// マップフィールドのセッタ
	/// </summary>
	/// <param name="mapChipField"></param>
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

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
	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

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
	static inline const float kGravityAcceleration = 0.04f;
	// 最大落下速度（下方向）
	static inline const float kLimitFallSpeed = 0.35f;
	// ジャンプ初速（上方向）
	static inline const float kJumpAcceleration = 0.5f;




	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;
};
