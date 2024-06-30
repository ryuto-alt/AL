#define NOMINMAX
#include "Player.h"
#include "MapChipField.h"
#include <Input.h>
#include <algorithm>
#include <cassert>
#include <numbers>

// 角
enum Corner {
	kRightBottom, // 右下
	kLeftBottom,  // 左下
	kRightTop,    // 右上
	kLeftTop,     // 左上
	kNumCorner    // 要素数
};

// 初期化
void Player::Initialize(Model* model, ViewProjection* viewProjection, const Vector3& position) {
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	// textureHandle_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// プレイヤーの大きさ
	worldTransform_.scale_ = {1.0f, 1.0f, 1.0f};

	// 初期回転
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	// プレイヤーの座標
	worldTransform_.translation_ = position;

	// 引数の内容をメンバ変数に記録
	viewProjection_ = viewProjection;
}

// 更新処理
void Player::Update() {
	/*-----①移動入力-----*/
	Move();

	/*-----②移動量を加味して衝突判定する-----*/
	// 衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	// 移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;

	// ②マップ衝突チェック
	MapCollisionDetection(collisionMapInfo);

	BottomDetection(collisionMapInfo);

	// 左右----
	LeftCollisionDetection(collisionMapInfo);
	RightCollisionDetection(collisionMapInfo);
	// ----

	GroundDetection(collisionMapInfo);

	/*-----③判定結果を反映して移動させる-----*/
	MoveReflectJudgmentResult(collisionMapInfo);

	/*-----④天井に接触している場合の処理-----*/
	TouchingCeiling(collisionMapInfo);

	/*-----⑤壁に接触している場合の処理-----*/

	/*-----⑥接地状態の切り替え-----*/

	/*-----⑦旋回制御-----*/
	TurningControl();
	/*-----⑧行列計算-----*/
	// 行列を定数バッファに転送
	// worldTransform_.TransferMatrix();
	worldTransform_.UpdateMatrix();
}

// 描画処理
void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *viewProjection_);
}

// 関数化：指定した角の座標計算
Vector3 CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
    };
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

// ①移動処理
void Player::Move() {
	//-----移動入力-----//
	// 接地状態
	if (onGround_) {
		// 慣性移動・左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				// 左移動中の右入力
				if (velocity_.x < 0.0f) {

					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				// 左右状態切り替え
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}

				acceleration.x += kAcceleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				// 右入力中の左入力
				if (velocity_.x > 0.0f) {

					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}

				// 左右状態切り替え
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}

				acceleration.x -= kAcceleration;
			}
			// 加速/減速
			velocity_.x += acceleration.x;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {

			// 日入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプの初速
			velocity_.y += kJumpAcceleration;
		}
	} else {
		// 落下速度
		velocity_.y += -kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0) {
		// Y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	// 接地判定
	if (onGround_) {
		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	} else {
		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 1.0f;
			// 摩擦で横方向に速度が減衰する
			velocity_.x *= (1.0f - kAttenuationLanding);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 接地状態に移行
			onGround_ = true;
		}
	}
}

// ②マップ衝突判定
void Player::MapCollisionDetection(CollisionMapInfo& info) {
	// 上昇アリ？
	if (info.move.y <= 0) {
		return;
	}
	// 移動後４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(0, info.move.y, 0), static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右上点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, info.move.y, 0) + Vector3(0, kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, (rect.bottom - worldTransform_.translation_.y) - ((kHeight / 2.0f) + kBlank));
		// 天井に当たったことを記録する
		info.isCeiling = true;
	}
}

#pragma region 下方向

// ②マップ衝突判定
void Player::BottomDetection(CollisionMapInfo& info) {
	// 下降アリ？
	if (info.move.y >= 0) {
		return;
	}
	// 移動後４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + Vector3(0, info.move.y, 0), static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 真上の当たり判定を行う
	bool hit = false;
	// 左下点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(0, info.move.y, 0) - Vector3(0, kHeight / 2.0f, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::min(0.0f, (rect.top - worldTransform_.translation_.y) + ((kHeight / 2.0f) + kBlank));
		// 地面に当たったことを記録する
		info.isLanding = true;
	}
}
#pragma endregion

void Player::GroundDetection(CollisionMapInfo& info) {
	std::array<Vector3, kNumCorner> positionNew;
	// 移動後の４つの角の座標を計算
	for (uint32_t i = 0; i < positionNew.size(); ++i) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	bool hit = false;

	// 左下点の判定
	Vector3 leftBottomCheckPos = positionNew[kLeftBottom] + Vector3(0, -0.4f, 0);
	MapChipField::IndexSet leftBottomIndex = mapChipField_->GetMapChipIndexSetByPosition(leftBottomCheckPos);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(leftBottomIndex.xIndex, leftBottomIndex.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// 右下点の判定
	Vector3 rightBottomCheckPos = positionNew[kRightBottom] + Vector3(0, -0.4f, 0);
	MapChipField::IndexSet rightBottomIndex = mapChipField_->GetMapChipIndexSetByPosition(rightBottomCheckPos);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(rightBottomIndex.xIndex, rightBottomIndex.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		onGround_ = true;
		// ジャンプ力を制限するために y 速度をリセット
		if (velocity_.y > 0) {
			velocity_.y = 0.0f;
		}
	} else {
		onGround_ = false;
	}
}

#pragma region 左方向当たり判定
// 左方向の衝突判定
void Player::LeftCollisionDetection(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.move.x >= 0.0f) {
		return;
	}
	// 移動後４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 右の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(info.move.x, 0, 0) + Vector3(kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.x = std::max(0.0f, (rect.left - worldTransform_.translation_.x) - ((kWidth / 2.0f) + kBlank));
		// 右壁に当たったことを記録する
		info.isWall = true;
	}
}
#pragma endregion

#pragma region 右方向当たり判定
// 右方向の衝突判定
void Player::RightCollisionDetection(CollisionMapInfo& info) {
	// 右移動あり？
	if (info.move.x <= 0.0f) {
		return;
	}
	// 移動後４つの角の座標
	std::array<Vector3, kNumCorner> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}
	MapChipType mapChipType;
	// 右の当たり判定を行う
	bool hit = false;
	// 右上点の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下点の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// ブロックにヒット？
	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + Vector3(info.move.x, 0, 0) + Vector3(kWidth / 2.0f, 0, 0));
		// めり込み先ブロックの範囲矩形
		Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);

		info.move.x = std::max(0.0f, (rect.left - worldTransform_.translation_.x) - ((kWidth / 2.0f) + kBlank));
		// 右壁に当たったことを記録する
		info.isWall = true;
	}
}
#pragma endregion

// ③判定結果を反映して移動させる
void Player::MoveReflectJudgmentResult(CollisionMapInfo& info) {
	// 衝突判定後の移動量を反映
	worldTransform_.translation_ += info.move;
}

// ④天井に接触している場合の処理
void Player::TouchingCeiling(CollisionMapInfo& info) {
	// 天井に当たった？
	if (info.isCeiling) {
		OutputDebugString(L"hit");
		// 上方向への速度をゼロにする
		velocity_.y = 0;
	}
}

// ⑦旋回制御
void Player::TurningControl() {
	// 旋回制御
	if (turnTimer_ > 0.0f) {

		// 旋回タイマーを1/60秒分カウントダウンする
		turnTimer_ -= static_cast<float>(1) / 60;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 角度補間による自キャラの角度設定
		// なめらかな角度の遷移を行う
		float t = 1.0f - (turnTimer_ / kTimeTurn);
		float interpolatedRotationY = Lerp(turnFirstRotationY_, destinationRotationY, t);
		worldTransform_.rotation_.y = interpolatedRotationY;
	}
}

void Player::OnHitByEnemy() {
	velocity_.y = 1.0f; // 高くジャンプさせる
}
