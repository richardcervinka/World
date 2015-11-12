#pragma once

#include <cmath>
#include <DirectXMath.h>

namespace Math {
	
	// constants
	const float PI = DirectX::XM_PI;
	const float PI2 = DirectX::XM_PI * 2.0f;
	const float PIDIV2 = DirectX::XM_PIDIV2;

	inline float Clamp( const float value, const float min, const float max ) {
		return fminf( fmaxf( value, min ), max );
	}
	
	//template< typename T>
	inline int Abs( const int value ) {
		return abs( value );
	}

	template< typename T>
	inline T Max( const T& a, const T& b ) {
		return a > b ? a : b;
	}

	template< typename T>
	inline T Max( const T& a, const T& b, const T& c ) {
		return std::max( std::max( a, b ), c );
	}

	template< typename T>
	inline T Max( const T& a, const T& b, const T& c, const T& d ) {
		return std::max( std::max( a, b ), std::max( c, d ) );
	}

	template< typename T>
	inline T Min( const T& a, const T& b ) {
		return a < b ? a : b;
	}

	template< typename T>
	inline T Min( const T& a, const T& b, const T& c ) {
		return std::min( std::min( a, b ), c );
	}

	template< typename T>
	inline T Min( const T& a, const T& b, const T& c, const T& d ) {
		return std::min( std::min( a, b ), std::min( c, d ) );
	}

	inline float Max( const float a, const float b ) {
		return std::fmax( a, b );
	}
	
	inline float Max( const float a, const float b, const float c ) {
		return std::fmax( fmax( a, b ), c );
	}
	
	inline float Max( const float a, const float b, const float c, const float d ) {
		return std::fmax( std::fmax( a, b ), std::fmax( c, d ) );
	}
	
	inline float Min( const float a, const float b ) {
		return std::fmin( a, b );
	}
	
	inline float Min( const float a, const float b, const float c ) {
		return std::fmin( std::fmin( a, b ), c );
	}
	
	inline float Min( const float a, const float b, const float c, const float d ) {
		return std::fmin( std::fmin( a, b ), std::fmin( c, d ) );
	}

	// goniometric functions

	inline float Sin( const float rad ) {
		return DirectX::XMScalarSin( rad );
	}

	inline float Cos( const float rad ) {
		return DirectX::XMScalarCos( rad );
	}

	inline float ASin( const float value ) {
		return DirectX::XMScalarASin( value );
	}

	inline float ACos( const float value ) {
		return DirectX::XMScalarACos( value );
	}

	inline float Atan2( const float y, const float x ) {
		return atan2f( y, x );
	}

	// scalar functions

	inline float Log2( const float x ) {
		return log2( x );
	}
	
	inline float Pow( const float base, const float exponent ) {
		return pow( base, exponent );
	}
	
	inline int Pow2( const int exponent ) {
		return 0x01 << static_cast< unsigned int >( exponent );
	}

	// help functions

	inline bool IsPow2( const int value ) {
		if ( value <= 0 ) {
			return false;
		}
		return ( value & ( value - 1 ) ) == 0;
	}

	int NearestUpperPow2( const int value );
	int NearestLowerPow2( const int value );
}