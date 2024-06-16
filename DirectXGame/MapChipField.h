#pragma once
#include <Vector3.h>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

/// <summary>
///  マップチップ種別の定義
/// </summary>
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};

/// <summary>
///  マップチップデータの構造体
/// </summary>
struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

// 矩形範囲
struct Rect {
	float left;   // 左端
	float right;  // 右端
	float bottom; // 下端
	float top;    // 上端
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
	// 1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	// ブロックの個数
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;
	// マップチップデータの構造体
	MapChipData mapChipData_;

public:
	struct IndexSet {
		uint32_t xIndex;
		uint32_t yIndex;
	};

	/// <summary>
	/// マップチップデータリセット
	/// </summary>
	void ResetMapChipData();

	/// <summary>
	/// マップチップCSV読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void LoadMapChipCsv(const std::string& filePath);

	int GetNumBlockVirtical() { return kNumBlockVirtical; }
	int GetNumBlockHorizontal() { return kNumBlockHorizontal; }

	/// <summary>
	/// マップチップ種別の取得
	/// </summary>
	/// <param name="xIndex"></param>
	/// <param name="yIndex"></param>
	/// <returns></returns>
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// マップチップ座標の取得
	/// </summary>
	/// <param name="xIndex"></param>
	/// <param name="yIndex"></param>
	/// <returns></returns>
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// 座標からマップチップ番号を計算
	/// </summary>
	/// <param name="position"></param>
	/// <returns></returns>
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	/// <summary>
	/// ブロックの範囲取得関数
	/// </summary>
	/// <param name="xIndex"></param>
	/// <param name="yIndex"></param>
	/// <returns></returns>
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);
};
