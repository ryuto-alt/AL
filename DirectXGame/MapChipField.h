#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <Vector3.h>

class MapChipField {
public:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;

	void LoadMapChipCsv(const std::string& filepath);                    // 宣言を追加
	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex); // 宣言を追加
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex); // 宣言を追加

private:
	void ResetMapChipData(); // ResetMapChipData関数の宣言を追加
};

enum class MapChipType {
	kBlank,
	kBlock,
};

struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};
