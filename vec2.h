#ifndef __VEC2_H
#define __VEC2_H

class vec2 {
	// 2d vector class
	float xy[2];

	bool lengthCached;
	float C_length;
	float C_length2;
	
	public:
	
	vec2() {
		xy[0] = 0.f;
		xy[1] = 0.f;
		lengthCached = false;
	}

	vec2(float x, float y) {
		xy[0] = x;
		xy[1] = y;
		lengthCached = false;	
	}

	__forceinline void set(float x, float y) {
		xy[0] = x;
		xy[1] = y;
		lengthCached = false;
	}

	__forceinline float& operator[](const int index) {
		lengthCached = false;
		return xy[index];
	}

	__forceinline const float& operator[](const int index) const {
		return xy[index];	
	}

	__forceinline void negate() {
		lengthCached = false;
		xy[0] *= -1.f;
		xy[1] *= -1.f;
	}

	__forceinline vec2& operator=(const vec2& v) {
		if(this == &v) return *this;
		lengthCached = false;
		xy[0] = v[0];
		xy[1] = v[1];
		return *this;
	}
	__forceinline bool operator==(const vec2& v) const {
		if(xy[0] == v[0] && xy[1] == v[1]) return true;
		return false;	
	}

	__forceinline  bool operator!=(const vec2& v) {
		return !(*this == v);
	}

	__forceinline vec2& operator+=(const vec2& v) {
		lengthCached = false;
		xy[0] += v[0];
		xy[1] += v[1];
		return *this;
	}

	__forceinline vec2& operator-=(const vec2& v) {
		lengthCached = false;
		vec2 tmp(v);
		tmp.negate();
		*this += tmp;
		return *this;
	}

	__forceinline vec2& operator*=(const float& x) {
		lengthCached = false;
		xy[0] *= x;
		xy[1] *= x;
		return *this;
	}

	__forceinline vec2 operator+(const vec2& v) const {
		return vec2(*this) += v;
	}

	__forceinline vec2 operator-(const vec2& v) const {
		return vec2(*this) -= v;
	}

	__forceinline vec2 operator*(const float& x) const {
		return vec2(*this) *= x;
	}

	__forceinline float dot_product(const vec2& v) const {
		return (xy[0] * v[0] + xy[1] * v[1]);
	}

	__forceinline float len() {
		if(!lengthCached) {
			C_length2 = xy[0] * xy[0] + xy[1] * xy[1];
			C_length = std::sqrt(C_length2);
			lengthCached = true;
		}
		return C_length;
	}

	__forceinline float len2() {
		if(!lengthCached) {
			C_length2 = xy[0] * xy[0] + xy[1] * xy[1];
		}
		return C_length2;
	}

	__forceinline void normalize() {
		float len = this->len();
		xy[0] /= len;
		xy[1] /= len;
		lengthCached = true;
		C_length2 = C_length = 1.f;
	}

	__forceinline vec2 perp() const {
		return vec2(-xy[1], xy[0]);
	}
};

#endif