#pragma once

#include <DirectXMath.h>
#include "Types.h"

class Matrix;

// do aritmetickych operaci se nezahrnuje w
//
class alignas( 16 ) Vector {
public:
	union {
		struct {
			float x, y, z, w;
		};
		// internal use only!
		DirectX::XMFLOAT4A xm4;
	};
	
public:
	Vector();
	Vector( const float x, const float y, const float z, const float w );
	explicit Vector( const float replicate );
	Vector( const Vector& begin, const Vector& end );
	
	// pretypovani z FloatN
	explicit Vector( const Float2& src );
	explicit Vector( const Float3& src );
	explicit Vector( const Float4& src );
	
	// pretypovani na FloatN
	operator Float2() const;
	operator Float3() const;
	operator Float4() const;
	
	// ulozeni do FloatN (neni potreba vytvaret docasny objekt)
	void Store( Float2& dest ) const;
	void Store( Float3& dest ) const;
	void Store( Float4& dest ) const;
	
	// setters
	void Set( const float x, const float y, const float z, const float w );
	void Set( const float replicate );
	
	// aritmeticke operace
	void Add( const Vector& v );
	void Add( const float value );
	void Sub( const Vector& v );
	void Sub( const float value );
	void Mul( const Vector& v );
	void Mul( const float value );
	void Div( const Vector& v );
	void Div( const float value );
	
	// aritmeticke operatory
	Vector operator+( const Vector& v ) const;
	Vector operator+( const float value ) const;
	Vector& operator+=( const Vector& v );
	Vector& operator+=( const float value );
	Vector operator-( const Vector& v ) const;
	Vector operator-( const float value ) const;
	Vector& operator-=( const Vector& v );
	Vector& operator-=( const float value );
	Vector operator*( const Vector& v ) const;
	Vector operator*( const float value ) const;
	Vector& operator*=( const Vector& v );
	Vector& operator*=( const float value );
	Vector operator/( const Vector& v ) const;
	Vector operator/( const float value ) const;
	Vector& operator/=( const Vector& v );
	Vector& operator/=( const float value );
	
	// porovnani
	bool operator==( const Vector& v ) const;
	bool operator!=( const Vector& v ) const;
	
	// 3D operace
	float Dot( const Vector& v ) const;
	void Cross( const Vector& v );
	void Normalize();
	
	// 3D transformace
	void Transform( const Matrix& matrix );
	void Rotate( const float pitch, const float yaw, const float roll );
	
	// velikost 3D vektoru
	float GetLength() const;
	float GetLengthSq() const;
	
	// operace mezi dvema vektory
	static float Distance( const Vector& begin, const Vector& end );
	static float DistanceSq( const Vector& begin, const Vector& end );
};

#include "Matrix.h"

inline Vector::Vector() {
	Set( 0, 0, 0, 0 );
}

inline Vector::Vector( const float x, const float y, const float z, const float w ) {
	Set( x, y, z, w );
}

inline Vector::Vector( const float replicate ) {
	x = y = z = replicate;
	w = 0;
}

inline Vector::Vector( const Vector& begin, const Vector& end ) {
	x = end.x - begin.x;
	y = end.y - begin.y;
	z = end.z - begin.z;
	w = 0;
}

inline Vector::Vector( const Float2& src ) {
	x = src.x;
	y = src.y;
	z = w = 0;
}

inline Vector::Vector( const Float3& src ) {
	x = src.x;
	y = src.y;
	z = src.z;
	w = 0;
}

inline Vector::Vector( const Float4& src ) {
	x = src.x;
	y = src.y;
	z = src.z;
	w = src.w;
}

inline Vector::operator Float2() const {
	return Float2( x, y );
}

inline Vector::operator Float3() const {
	return Float3( x, y, z );
}

inline Vector::operator Float4() const {
	return Float4( x, y, z, w );
}

inline void Vector::Store( Float2& dest ) const {
	dest.x = x;
	dest.y = y;
}

inline void Vector::Store( Float3& dest ) const {
	dest.x = x;
	dest.y = y;
	dest.z = z;
}

inline void Vector::Store( Float4& dest ) const {
	dest.x = x;
	dest.y = y;
	dest.z = z;
	dest.w = w;
}

inline void Vector::Set( const float x, const float y, const float z, const float w ) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

inline void Vector::Set( const float replicate ) {
	x = y = z = replicate;
	w = 0;
}

inline void Vector::Add( const Vector& v ) {
	x += v.x;
	y += v.y;
	z += v.z;
}

inline void Vector::Add( const float value ) {
	x += value;
	y += value;
	z += value;
}

inline void Vector::Sub( const Vector& v ) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
}

inline void Vector::Sub( const float value ) {
	x -= value;
	y -= value;
	z -= value;
}

inline void Vector::Mul( const Vector& v ) {
	x *= v.x;
	y *= v.y;
	z *= v.z;
}

inline void Vector::Mul( const float value ) {
	x *= value;
	y *= value;
	z *= value;
}

inline void Vector::Div( const Vector& v ) {
	x /= v.x;
	y /= v.y;
	z /= v.z;
}

inline void Vector::Div( const float value ) {
	x /= value;
	y /= value;
	z /= value;
}

inline float Vector::Dot( const Vector& v ) const {
	return x * v.x + y * v.y + z * v.z;
}

inline void Vector::Cross( const Vector& v ) {
	DirectX::XMStoreFloat4A( &xm4,
		DirectX::XMVector3Cross(
			DirectX::XMLoadFloat4A( &xm4 ),
			DirectX::XMLoadFloat4A( &v.xm4 )
		)
	);
}

inline void Vector::Normalize() {
	DirectX::XMStoreFloat4A( &xm4,
		DirectX::XMVector3Normalize(
			DirectX::XMLoadFloat4A( &xm4 )
		)
	);
}

inline void Vector::Transform( const Matrix& matrix ) {
	DirectX::XMStoreFloat4A( &xm4,
		DirectX::XMVector3Transform(
			DirectX::XMLoadFloat4A( &xm4 ),
			DirectX::XMLoadFloat4x4A( &matrix.xm4x4 )
		)
	);
	// MSDN XMVector3Transform(): The w component of the returned vector may be non-homogeneous (!= 1.0)
	w = 0;
}

inline void Vector::Rotate( const float pitch, const float yaw, const float roll ) {
	DirectX::XMStoreFloat4A( &xm4,
		DirectX::XMVector3Rotate(
			DirectX::XMLoadFloat4A( &xm4 ),
			DirectX::XMQuaternionRotationRollPitchYaw( pitch, yaw, roll )
		)
	);
}

inline Vector Vector::operator+( const Vector& v ) const {
	Vector vector = *this;
	vector.Add( v );
	return vector;
}

inline Vector Vector::operator+( const float value ) const {
	Vector vector = *this;
	vector.Add( value );
	return vector;
}

inline Vector& Vector::operator+=( const Vector& v ) {
	this->Add( v );
	return *this;
}

inline Vector& Vector::operator+=( const float value ) {
	this->Add( value );
	return *this;
}

inline Vector Vector::operator-( const Vector& v ) const {
	Vector vector = *this;
	vector.Sub( v );
	return vector;
}

inline Vector Vector::operator-( const float value ) const {
	Vector vector = *this;
	vector.Sub( value );
	return vector;
}

inline Vector& Vector::operator-=( const Vector& v ) {
	this->Sub( v );
	return *this;
}

inline Vector& Vector::operator-=( const float value ) {
	this->Sub( value );
	return *this;
}

inline Vector Vector::operator*( const Vector& v ) const {
	Vector vector = *this;
	vector.Mul( v );
	return vector;
}

inline Vector Vector::operator*( const float value ) const {
	Vector vector = *this;
	vector.Mul( value );
	return vector;
}

inline Vector& Vector::operator*=( const Vector& v ) {
	this->Mul( v );
	return *this;
}

inline Vector& Vector::operator*=( const float value ) {
	this->Mul( value );
	return *this;
}

inline Vector Vector::operator/( const Vector& v ) const {
	Vector vector = *this;
	vector.Div( v );
	return vector;
}

inline Vector Vector::operator/( const float value ) const {
	Vector vector = *this;
	vector.Div( value );
	return vector;
}

inline Vector& Vector::operator/=( const Vector& v ) {
	this->Div( v );
	return *this;
}

inline Vector& Vector::operator/=( const float value ) {
	this->Div( value );
	return *this;
}

inline bool Vector::operator==( const Vector& v ) const {
	return ( x == v.x ) && ( y == v.y ) && ( z == v.z ) && ( w == v.w );
}

inline bool Vector::operator!=( const Vector& v ) const {
	return ( x != v.x ) || ( y != v.y ) || ( z != v.z ) || ( w != v.w );
}

inline float Vector::GetLength() const {
	return sqrtf( x * x + y * y + z * z );
}

inline float Vector::GetLengthSq() const {
	return x * x + y * y + z * z;
}

inline float Vector::Distance( const Vector& begin, const Vector& end ) {
	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat4A( &end.xm4 ),
		DirectX::XMLoadFloat4A( &begin.xm4 )
	);
	return DirectX::XMVectorGetX( DirectX::XMVector3Length( diff ) );
}

inline float Vector::DistanceSq( const Vector& begin, const Vector& end ) {
	DirectX::XMVECTOR diff = DirectX::XMVectorSubtract(
		DirectX::XMLoadFloat4A( &end.xm4 ),
		DirectX::XMLoadFloat4A( &begin.xm4 )
	);
	return DirectX::XMVectorGetX( DirectX::XMVector3LengthSq( diff ) );
}