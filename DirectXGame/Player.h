#pragma once
#include "Model.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include <DebugCamera.h>
class Player {

public:
	// 初期化

	void Initialize(Model* model, uint32_t textureHandle, ViewProjection* viewProjection);

	// 更新

	void Update();

	// 描画

	void Draw();

private:
	// ワールド変換データ
	WorldTransform worldTransform;

	// モデル
	Model* model = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle = 0u;

	ViewProjection* viewProjection = nullptr;

	DebugCamera* debugCamera_ = nullptr;
};
