#pragma once

struct Quaternion {
	float x;
	float y;
	float z;
	float w;
	Quaternion& operator*=(const float s) {
		x *= s;
		y *= s;
		z *= s;
		w *= s;
		return *this;
	}
};
