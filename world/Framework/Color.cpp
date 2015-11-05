#include "Color.h"
#include "Math.h"

// class ColorUnorm

const ColorUnorm ColorUnorm::BLACK = ColorUnorm( 0, 0, 0, 255 );
const ColorUnorm ColorUnorm::WHITE = ColorUnorm( 255, 255, 255, 255 );
const ColorUnorm ColorUnorm::RED = ColorUnorm( 255, 0, 0, 255 );
const ColorUnorm ColorUnorm::GREEN = ColorUnorm( 0, 255, 0, 255 );
const ColorUnorm ColorUnorm::BLUE = ColorUnorm( 0, 0, 255, 255 );
const ColorUnorm ColorUnorm::CYAN = ColorUnorm( 0, 255, 255, 255 );
const ColorUnorm ColorUnorm::MAGENTA = ColorUnorm( 255, 0, 255, 255 );
const ColorUnorm ColorUnorm::YELLOW = ColorUnorm( 255, 255, 0, 255 );

ColorUnorm::ColorUnorm( const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

ColorUnorm::ColorUnorm( const uint32_t rgba ) {
	pack = rgba;
}

bool ColorUnorm::operator==( const ColorUnorm& color ) {
	return pack == color.pack;
}

bool ColorUnorm::operator!=( const ColorUnorm& color ) {
	return pack != color.pack;
}

// class Color

const Color Color::BLACK = Color( 0, 0, 0, 1.0f );
const Color Color::WHITE = Color( 1.0f, 1.0f, 1.0f, 1.0f );
const Color Color::RED = Color( 1.0f, 0, 0, 1.0f );
const Color Color::GREEN = Color( 0, 1.0f, 0, 1.0f );
const Color Color::BLUE = Color( 0, 0, 1.0f, 1.0f );
const Color Color::CYAN = Color( 0, 1.0f, 1.0f, 1.0f );
const Color Color::MAGENTA = Color( 1.0f, 0, 1.0f, 1.0f );
const Color Color::YELLOW = Color( 1.0f, 1.0f, 0, 1.0f );

Color::Color() {
	r = g = b = 0.0f;
	a = 1.0f;
}

Color::Color( const float r, const float g, const float b, const float a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

Color::Color( const Vector& vector ) {
	r = vector.x;
	g = vector.y;
	b = vector.z;
	a = vector.w;
}

Color::Color( const Float4& f ) {
	r = f.x;
	g = f.y;
	b = f.z;
	a = f.w;
}

Color::Color( const Float3& f ) {
	r = f.x;
	g = f.y;
	b = f.z;
	a = 1.0f;
}

Color::Color( const ColorUnorm& color ) {
	r = static_cast< float >( color.r ) / 255.0f;
	g = static_cast< float >( color.g ) / 255.0f;
	b = static_cast< float >( color.b ) / 255.0f;
	a = static_cast< float >( color.a ) / 255.0f;
}

void Color::SetRGBA( const float r, const float g, const float b, const float a ) {
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

ColorUnorm Color::ToUnorm() const {
	return ColorUnorm(
		static_cast< uint8_t >( Math::Clamp( r * 255.0f, 0.0f, 255.0f ) ),
		static_cast< uint8_t >( Math::Clamp( g * 255.0f, 0.0f, 255.0f ) ),
		static_cast< uint8_t >( Math::Clamp( b * 255.0f, 0.0f, 255.0f ) ),
		static_cast< uint8_t >( Math::Clamp( a * 255.0f, 0.0f, 255.0f ) )
	);
}

Vector Color::ToVector() const {
	return Vector( r, g, b, a );
}

Float4 Color::ToFloat4() const {
	return Float4( r, g, b, a );
}

Float3 Color::ToFloat3() const {
	return Float3( r, g, b );
}

void Color::StoreUnorm( ColorUnorm& target ) const {
	target.r = static_cast< uint8_t >( Math::Clamp( r * 255.0f, 0.0f, 255.0f ) );
	target.g = static_cast< uint8_t >( Math::Clamp( g * 255.0f, 0.0f, 255.0f ) );
	target.b = static_cast< uint8_t >( Math::Clamp( b * 255.0f, 0.0f, 255.0f ) );
	target.a = static_cast< uint8_t >( Math::Clamp( a * 255.0f, 0.0f, 255.0f ) );
}

void Color::StoreVector( Vector& target ) const {
	target.Set( r, g, b, a ); 
}

void Color::StoreFloat4( Float4& target ) const {
	target.x = r;
	target.y = g;
	target.z = b;
	target.w = a;
}

void Color::StoreFloat3( Float3& target ) const {
	target.x = r;
	target.y = g;
	target.z = b;
}

void Color::Clamp() {
	r = Math::Clamp( r, 0.0f, 1.0f );
	g = Math::Clamp( g, 0.0f, 1.0f );
	b = Math::Clamp( b, 0.0f, 1.0f );
	a = Math::Clamp( a, 0.0f, 1.0f );
}

bool Color::operator==( const Color& color ) {
	return ( r == color.r ) && ( g == color.g ) && ( b == color.b ) && ( a == color.a );
}

bool Color::operator!=( const Color& color ) {
	return ( r != color.r ) || ( g != color.g ) || ( b != color.b ) || ( a != color.a );
}

void Color::SetHSV( const float h, const float s, const float v ) {
	// oriznout vstupni hodnoty do rozsahu <0; 1>
	float hc = Math::Clamp( h, 0.0f, 1.0f );
	float sc = Math::Clamp( s, 0.0f, 1.0f );
	float vc = Math::Clamp( v, 0.0f, 1.0f );
	
	float hdeg = float( int( hc * 360.0f ) % 360 );
	int hi = int( hdeg / 60.0f ) % 6;
	float f = ( hdeg / 60.0f ) - float( hi );
	float p = vc * ( 1.0f - sc );
	float q = vc * ( 1.0f - f * sc );
	float t = vc * ( 1.0f - ( 1.0f - f ) * sc );
	
	switch ( hi ) {
	case 0: SetRGBA( vc, t, p, 1.0f ); return;
	case 1: SetRGBA( q, vc, p, 1.0f ); return;
	case 2: SetRGBA( p, vc, t, 1.0f ); return;
	case 3: SetRGBA( p, q, vc, 1.0f ); return;
	case 4: SetRGBA( t, p, vc, 1.0f ); return;
	case 5: SetRGBA( vc, p, q, 1.0f ); return;
	};
}

Vector Color::GetHSV() const {
	// hodnoty rgba museji byt v rozsahu <0; 1>
	Color color = *this;
	color.Clamp();
	
	float maxc = Math::Max( color.r, color.g, color.b );
	float minc = Math::Min( color.r, color.g, color.b );
	
	// Hue
	float h = 0.0f;
	if ( maxc == minc ) {
		h = 0;
	} else if ( maxc == color.r ) {
		if ( color.g >= color.b ) {
			h = 60.0f * ( color.g - color.b ) / ( maxc - minc );
		} else {
			h = 60.0f * ( color.g - color.b ) / ( maxc - minc ) + 360.0f;
		}
	} else if ( maxc == color.g ) {
		h = 60.0f * ( color.b - color.r ) / ( maxc - minc ) + 120.0f;
	} else if ( maxc == color.b ) {
		h = 60.0f * ( color.r - color.g ) / ( maxc - minc ) + 240.0f;
	}
	// Saturation
	float s = 0.0f;
	if ( maxc != 0 ) {
		s = 1.0f - minc / maxc;
	}
	return Vector( h / 360.0f, s, maxc, color.a );
}