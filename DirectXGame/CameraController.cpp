#define NOMINMAX
#include "CameraController.h"
#include "Player.h"
#include <algorithm>

void CameraController::Initialize(ViewProjection* viewProjection) {
	// 引数の内容をメンバ変数に記録
	viewProjection_ = viewProjection;
}

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform* targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	goalPosition_.x = targetWorldTransform->translation_.x + targetOffset_.x + target_->GetVelocity().x * kVelocityBias;
	goalPosition_.y = targetWorldTransform->translation_.y + targetOffset_.y + target_->GetVelocity().y * kVelocityBias;

	// 座標補間によりゆったり追従
	viewProjection_->translation_.x = Lerp(viewProjection_->translation_.x, goalPosition_.x, kInterpolationRate);
	viewProjection_->translation_.y = Lerp(viewProjection_->translation_.y, goalPosition_.y, kInterpolationRate);

	viewProjection_->translation_.y = targetWorldTransform->translation_.y + targetOffset_.y;
	// 追従対象が画面買いに出ないように補正
	viewProjection_->translation_.x = std::max(viewProjection_->translation_.x, target_->GetWorldTransform()->translation_.x + margin.left);
	viewProjection_->translation_.x = std::min(viewProjection_->translation_.x, target_->GetWorldTransform()->translation_.x + margin.right);
	viewProjection_->translation_.y = std::max(viewProjection_->translation_.y, target_->GetWorldTransform()->translation_.y + margin.bottm);
	viewProjection_->translation_.y = std::min(viewProjection_->translation_.y, target_->GetWorldTransform()->translation_.y + margin.top);
	// 移動範囲制限
	viewProjection_->translation_.x = std::max(viewProjection_->translation_.x, movableArea_.left);
	viewProjection_->translation_.x = std::min(viewProjection_->translation_.x, movableArea_.right);
	viewProjection_->translation_.y = std::max(viewProjection_->translation_.y, movableArea_.bottm);
	viewProjection_->translation_.y = std::min(viewProjection_->translation_.y, movableArea_.top);

	// 行列を更新する
	viewProjection_->UpdateMatrix();
}

void CameraController::Reset() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform* targetWorldTransform = target_->GetWorldTransform();
	// 追従対象とオフセットからカメラの座標を計算
	viewProjection_->translation_.x = targetWorldTransform->translation_.x + targetOffset_.x;
	viewProjection_->translation_.y = targetWorldTransform->translation_.y + targetOffset_.y;
}
