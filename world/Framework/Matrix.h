#pragma once

#include <DirectXMath.h>
#include "Types.h"

class Vector;

// row major 4x4 matrix

class alignas( 16 ) Matrix {
public:
	union {
		struct {
			float m00, m01, m02, m03;
			float m10, m11, m12, m13;
			float m20, m21, m22, m23;
			float m30, m31, m32, m33;
		};
		float m[ 4 ][ 4 ];
		
		// internal use only!
		DirectX::XMFLOAT4X4A xm4x4;
	};

public:
	Matrix();
	
	// operace
	void Mul( const Matrix &m );
	void Identity();
	void Transpose();
	void Inverse();
	
	// transformace
	void Transform( const Matrix &transformations );
	void Translate( const float x, const float y, const float z );
	void Translate( const Vector &xyz );
	void Rotate( const float roll, const float pitch, const float yaw );
	void Rotate( const Vector &rollPitchYaw );
	void RotateX( const float rad );
	void RotateY( const float rad );
	void RotateZ( const float rad );
	void RotateAxis( const Vector &axes, const float rad );
	void Scale( const float x, const float y, const float z );
	void Scale( const Vector &xyz );
	
	// ulozeni do FloatNxN jako column major
	void StoreColumnMajor( Float4x4 &dest ) const;
	void StoreColumnMajor( Float3x3 &dest ) const;
	
	// ulozeni do FloatNxN
	void Store( Float4x4 &dest ) const;
	void Store( Float3x3 &dest ) const;
	
	// pretypovani na FloatNxN
	operator Float4x4() const;
	operator Float3x3() const;
	
	// projekcni matice
	void PerspectiveLH( const float fovVertical, const float aspectRatio, const float nearDraw, const float farDraw );
	
	// pohledova matice
	void LookToLH( const Vector &position, const Vector &direction, const Vector &up );
};

#include "Vector.h"

inline Matrix::Matrix() {
	DirectX::XMStoreFloat4x4A( &xm4x4, DirectX::XMMatrixIdentity() );
}

inline void Matrix::Identity() {
	DirectX::XMStoreFloat4x4A( &xm4x4, DirectX::XMMatrixIdentity() );
}

inline void Matrix::Transpose() {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixTranspose(
			DirectX::XMLoadFloat4x4A( &xm4x4 )
		)
	);
}

inline void Matrix::Inverse() {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixInverse( nullptr, DirectX::XMLoadFloat4x4A( &xm4x4 ) )
	);
}

inline void Matrix::Mul( const Matrix &m ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply(
			DirectX::XMLoadFloat4x4A( &xm4x4 ),
			DirectX::XMLoadFloat4x4A( &m.xm4x4 )
		)
	);
}

inline void Matrix::Transform( const Matrix &transformations ) {
	Mul( transformations );
}

inline void Matrix::Translate( const float x, const float y, const float z ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply(
			DirectX::XMLoadFloat4x4A( &xm4x4 ),
			DirectX::XMMatrixTranslation( x, y, z )
		)
	);
}

inline void Matrix::Translate( const Vector &xyz ) {
	Translate( xyz.x, xyz.y, xyz.z );
}

inline void Matrix::Rotate( const float roll, const float pitch, const float yaw ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply(
			DirectX::XMLoadFloat4x4A( &xm4x4 ),
			DirectX::XMMatrixRotationRollPitchYaw( pitch, yaw, roll )
		)
	);
}

inline void Matrix::Rotate( const Vector &rollPitchYaw ) {
	Rotate( rollPitchYaw.y, rollPitchYaw.z, rollPitchYaw.x );
}

inline void Matrix::RotateX( const float rad ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply(
			DirectX::XMLoadFloat4x4A( &xm4x4 ),
			DirectX::XMMatrixRotationX( rad )
		)
	);
}

inline void Matrix::RotateY( const float rad ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply(
			DirectX::XMLoadFloat4x4A( &xm4x4 ),
			DirectX::XMMatrixRotationY( rad )
		)
	);
}

inline void Matrix::RotateZ( const float rad ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply(
			DirectX::XMLoadFloat4x4A( &xm4x4 ),
			DirectX::XMMatrixRotationZ( rad )
		)
	);
}

inline void Matrix::RotateAxis( const Vector &axes, const float rad ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply(
			DirectX::XMLoadFloat4x4A( &xm4x4 ),
			DirectX::XMMatrixRotationAxis( DirectX::XMLoadFloat4( &axes.xm4 ), rad )
		)
	);
}

inline void Matrix::Scale( const float x, const float y, const float z ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixMultiply( 
			DirectX::XMLoadFloat4x4A( &xm4x4 ), 
			DirectX::XMMatrixScaling( x, y, z )
		)
	);
}

inline void Matrix::Scale( const Vector &xyz ) {
	Scale( xyz.x, xyz.y, xyz.z );
}

inline void Matrix::StoreColumnMajor( Float4x4 &dest ) const {
	DirectX::XMStoreFloat4x4A(
		reinterpret_cast< DirectX::XMFLOAT4X4A* >( &dest ),
		DirectX::XMMatrixTranspose( DirectX::XMLoadFloat4x4A( &xm4x4 ) )
	);
}

inline void Matrix::StoreColumnMajor( Float3x3 &dest ) const {
	DirectX::XMStoreFloat3x3(
		reinterpret_cast< DirectX::XMFLOAT3X3* >( &dest ),
		DirectX::XMMatrixTranspose( DirectX::XMLoadFloat4x4A( &xm4x4 ) )
	);
}

inline void Matrix::Store( Float4x4 &dest ) const {
	DirectX::XMStoreFloat4x4A(
		reinterpret_cast< DirectX::XMFLOAT4X4A* >( &dest ),
		DirectX::XMLoadFloat4x4A( &xm4x4 )
	);
}

inline void Matrix::Store( Float3x3 &dest ) const {
	DirectX::XMStoreFloat3x3(
		reinterpret_cast< DirectX::XMFLOAT3X3* >( &dest ),
		DirectX::XMLoadFloat4x4A( &xm4x4 )
	);
}

inline Matrix::operator Float4x4() const {
	Float4x4 result;
	StoreColumnMajor( result );
	return result;
}

inline Matrix::operator Float3x3() const {
	Float3x3 result;
	StoreColumnMajor( result );
	return result;
}

inline void Matrix::PerspectiveLH( const float fovVertical, const float aspectRatio, const float nearDraw, const float farDraw ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixPerspectiveFovLH( fovVertical, aspectRatio, nearDraw, farDraw )
	);
}

inline void Matrix::LookToLH( const Vector &position, const Vector &direction, const Vector &up ) {
	DirectX::XMStoreFloat4x4A( &xm4x4,
		DirectX::XMMatrixLookToLH( 
			DirectX::XMLoadFloat4A( &position.xm4 ),
			DirectX::XMLoadFloat4A( &direction.xm4 ),
			DirectX::XMLoadFloat4A( &up.xm4 )
		)
	);
}