#pragma once

#include <DirectXMath.h>
//#include <cmath>

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
	inline T Max( const T &a, const T &b ) {
		return a > b ? a : b;
	}

	template< typename T>
	inline T Max( const T &a, const T &b, const T &c ) {
		return Max( Max( a, b ), c );
	}

	template< typename T>
	inline T Max( const T &a, const T &b, const T &c, const T &d ) {
		return Max( Max( a, b ), Max( c, d ) );
	}

	template< typename T>
	inline T Min( const T &a, const T &b ) {
		return a < b ? a : b;
	}

	template< typename T>
	inline T Min( const T &a, const T &b, const T &c ) {
		return Min( Min( a, b ), c );
	}

	template< typename T>
	inline T Min( const T &a, const T &b, const T &c, const T &d ) {
		return Min( Min( a, b ), Min( c, d ) );
	}

	inline float Max( const float a, const float b ) {
		return fmaxf( a, b );
	}
	
	inline float Max( const float a, const float b, const float c ) {
		return fmaxf( fmaxf( a, b ), c );
	}
	
	inline float Max( const float a, const float b, const float c, const float d ) {
		return fmaxf( fmaxf( a, b ), fmaxf( c, d ) );
	}
	
	inline float Min( const float a, const float b ) {
		return fminf( a, b );
	}
	
	inline float Min( const float a, const float b, const float c ) {
		return fminf( fminf( a, b ), c );
	}
	
	inline float Min( const float a, const float b, const float c, const float d ) {
		return fminf( fminf( a, b ), fminf( c, d ) );
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

	// ------- PRIDAT VEKTOROVE ZPRACOVANI -> SIN PRO 4 HODNOTY... -------

	inline bool IsPow2( const int value ) {
		if ( value <= 0 ) {
			return false;
		}
		return ( value & ( value - 1 ) ) == 0;
	}

	int NearestUpperPow2( const int value );
	int NearestLowerPow2( const int value );
}