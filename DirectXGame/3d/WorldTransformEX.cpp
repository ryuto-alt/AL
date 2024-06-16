#include "MatrixMath.h"
#include "WorldTransform.h"

void WorldTransform::UpdateMatrix() {
	// スケール・回転・平行移動を合成して計算する
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);
	// 定数バッファに転送する
	TransferMatrix();
}