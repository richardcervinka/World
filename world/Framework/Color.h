#pragma once

#include "Types.h"
#include "Vector.h"

// 8 bit format barvy, zapouzdruje usporadani jednotlivych kanalu v pameti
//
class ColorUnorm {
public:
	union {
		// protoze little-endian, museji byt kanaly v opacnem poradi
		struct {
			uint8_t a, b, g, r;
		};
		// RGBA kanaly v 4 byte bloku
		uint32_t pack;
	};
	
public:
	ColorUnorm( const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a );
	ColorUnorm( const uint32_t rgba );
	bool operator==( const ColorUnorm& color );
	bool operator!=( const ColorUnorm& color );
	
	// pojmenovane barvy
	static const ColorUnorm BLACK;
	static const ColorUnorm WHITE;
	static const ColorUnorm RED;
	static const ColorUnorm GREEN;
	static const ColorUnorm BLUE;
	static const ColorUnorm CYAN;
	static const ColorUnorm MAGENTA;
	static const ColorUnorm YELLOW;
};

// Barva v linearnim prostoru, kanaly jsou v rozsahu <0; 1>
//
class Color {
public:
	float r, g, b, a;
	
public:
	Color();
	Color( const float r, const float g, const float b, const float a );
	
	// create from
	explicit Color( const ColorUnorm& color );
	explicit Color( const Vector& vector );
	explicit Color( const Float4& f );
	explicit Color( const Float3& f );
	
	void SetRGBA( const float r, const float g, const float b, const float a );
	
	// vytvori RGB barvu z HSV; h<0; 1); s<0;1>; v<0;1>
	void SetHSV( const float h, const float s, const float v );
	
	// vrati Vector( h<0; 1), s<0;1>, v<0;1>, a<0;1> );
	Vector GetHSV() const;
	
	// cast to
	ColorUnorm ToUnorm() const;
	Vector ToVector() const;
	Float4 ToFloat4() const;
	Float3 ToFloat3() const;
	
	// store to; store metody jsou rychlejsi nez cast metody, nevytvari se docasny objekt
	void StoreUnorm( ColorUnorm& target ) const;
	void StoreVector( Vector& target ) const;
	void StoreFloat4( Float4& target ) const;
	void StoreFloat3( Float3& target ) const;
	
	bool operator==( const Color& color );
	bool operator!=( const Color& color );
	
	// orizne hodnoty do rozsahu <0; 1>
	void Clamp();
	
	// pojmenovane barvy
	static const Color BLACK;
	static const Color WHITE;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color CYAN;
	static const Color MAGENTA;
	static const Color YELLOW;
};