#include "GameScene.h"
#include "AxisIndicator.h"
#include "PrimitiveDrawer.h"
#include "TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {
	delete sprite;
	delete model;
	delete debugCamera;
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	textureHandle = TextureManager::Load("mario.jpg");
	sprite = Sprite::Create(textureHandle, {100, 50});
	model = Model::Create();
	// 3dデータ読み込み
	worldTransform.Initialize();
	viewProjection.Initialize();

	// サウンドデータ
	soundDataHandle = audio_->LoadWave("fanfare.wav");
	// audio再生
	// audio_->PlayWave(soundDataHandle);

	// voiceHandle = audio_->PlayWave(soundDataHandle, true);

	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection);
	// デバッグカメラ
	debugCamera = new DebugCamera(100, 100);

	AxisIndicator::GetInstance()->SetVisible(true);

	AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera->GetViewProjection());
}

void GameScene::Update() {
	Vector2 position = sprite->GetPosition();
	//===スプライト動かす処理===
	position.x += 2.0f;
	position.y += 1.0f;

	sprite->SetPosition(position);
	// ===スプライト動かす処理===

	if (input_->TriggerKey(DIK_SPACE)) {

		audio_->StopWave(voiceHandle);
	}
#ifdef DEBUG

	ImGui::Begin("Debug1");
	// float3入力ボックス
	ImGui::InputFloat3("InputFloat3", inputFloat3);
	// float3スライダー
	ImGui::SliderFloat3("SliderFloat3", inputFloat3, 0.0f, 1.0f);
	// ImGui::ShowDemoWindow();

	ImGui::End();
#endif

	debugCamera->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	// ラインを描画する
	PrimitiveDrawer::GetInstance()->DrawLine3d(
	    {
	        0,
	        0,
	        0,
	    },
	    {0, 10, 0}, {1.0f, 5.0f, 0.0f, 1.0f});

	Model::PostDraw();
	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	// model->Draw(worldTransform, viewProjection, textureHandle);

	model->Draw(worldTransform, debugCamera->GetViewProjection(), textureHandle);

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	// sprite->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
