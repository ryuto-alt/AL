#pragma once

/// <summary>
/// 3次元ベクトル
/// </summary>
struct Vector3 final {
	float x;
	float y;
	float z;

	// 加算演算子の定義
	Vector3 operator+(const Vector3& other) const { return {x + other.x, y + other.y, z + other.z}; }

	Vector3& operator+=(const Vector3& other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	// 減算演算子の定義
	Vector3 operator-(const Vector3& other) const { return {x - other.x, y - other.y, z - other.z}; }

	Vector3& operator-=(const Vector3& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
};