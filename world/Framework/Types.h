#pragma once

#include <cstdint>
#include <climits>

#ifndef Byte
using Byte = uint8_t;
#endif

struct alignas( 16 ) Float2 {
	float x, y;
	
	Float2( const float x = 0, const float y = 0 ) {
		this->x = x;
		this->y = y;
	}
};

struct alignas( 16 ) Float3 {
	float x, y, z;
	
	Float3( const float x = 0, const float y = 0, const float z = 0 ) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

struct alignas( 16 ) Float4 {
	float x, y, z, w;
	
	Float4( const float x = 0, const float y = 0, const float z = 0, const float w = 0 ) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
};

union alignas( 16 ) Float2x2 {
	struct {
		float m00, m01;
		float m10, m11;
	};
	float m[ 2 ][ 2 ];
};

union alignas( 16 ) Float3x3 {
	struct {
		float m00, m01, m02;
		float m10, m11, m12;
		float m20, m21, m22;
	};
	float m[ 3 ][ 3 ];
};

union alignas( 16 ) Float4x4 {
	struct {
		float m00, m01, m02, m03;
		float m10, m11, m12, m13;
		float m20, m21, m22, m23;
		float m30, m31, m32, m33;
	};
	float m[ 4 ][ 4 ];
};