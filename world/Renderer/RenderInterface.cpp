#include "RenderInterface.h"
#include "DX11\DX11RenderInterface.h"

using namespace RenderInterface;

Device* RenderInterface::DX11CreateDevice( const DX11CreateDeviceParams& params ) {
	DX11Device* device = new DX11Device();
	if ( device == nullptr ) {
		return nullptr;
	}
	if ( !device->Create( params ) ) {
		delete device;
		return nullptr;
	}
	return device;
}

const FormatInfo RenderInterface::GetFormatInfo( const Format format ) {
	switch ( format ) {									//channels	chan.bytes	blockSize	blockBytes
	case Format::UNKNOWN:						return { 0,			0,			1,			0 };
	case Format::R32G32B32A32_FLOAT:			return { 4,			4,			1,			16 };
	case Format::R32G32B32A32_UINT:				return { 4,			4,			1,			16 };
	case Format::R32G32B32_FLOAT:				return { 3,			4,			1,			12 };
	case Format::R32G32B32_UINT:				return { 3,			4,			1,			12 };
	case Format::R32G32_FLOAT:					return { 2,			4,			1,			8 };
	case Format::R32G32_UINT:					return { 2,			4,			1,			8 };
	case Format::R32_FLOAT:						return { 1,			4,			1,			4 };
	case Format::R32_UINT:						return { 1,			4,			1,			4 };
	case Format::R16G16B16A16_FLOAT:			return { 4,			2,			1,			8 };
	case Format::R16G16B16A16_UINT:				return { 4,			2,			1,			8 };
	case Format::R16G16B16A16_UNORM:			return { 4,			2,			1,			8 };
	case Format::R16G16B16A16_SINT:				return { 4,			2,			1,			8 };
	case Format::R16G16B16A16_SNORM:			return { 4,			2,			1,			8 };
	case Format::R16G16_FLOAT:					return { 2,			2,			1,			4 };
	case Format::R16G16_UINT:					return { 2,			2,			1,			4 };
	case Format::R16G16_UNORM:					return { 2,			2,			1,			4 };
	case Format::R16G16_SINT:					return { 2,			2,			1,			4 };
	case Format::R16G16_SNORM:					return { 2,			2,			1,			4 };
	case Format::R16_FLOAT:						return { 1,			2,			1,			2 };
	case Format::R16_UINT:						return { 1,			2,			1,			2 };
	case Format::R16_UNORM:						return { 1,			2,			1,			2 };
	case Format::R16_SINT:						return { 1,			2,			1,			2 };
	case Format::R16_SNORM:						return { 1,			2,			1,			2 };
	case Format::R8G8B8A8_UINT:					return { 4,			1,			1,			4 };
	case Format::R8G8B8A8_UNORM:				return { 4,			1,			1,			4 };
	case Format::R8G8B8A8_SINT:					return { 4,			1,			1,			4 };
	case Format::R8G8B8A8_SNORM:				return { 4,			1,			1,			4 };
	case Format::R8G8_UINT:						return { 2,			1,			1,			2 };
	case Format::R8G8_UNORM:					return { 2,			1,			1,			2 };
	case Format::R8G8_SINT:						return { 2,			1,			1,			2 };
	case Format::R8G8_SNORM:					return { 2,			1,			1,			2 };
	case Format::R8_UINT:						return { 1,			1,			1,			1 };
	case Format::R8_UNORM:						return { 1,			1,			1,			1 };
	case Format::R8_SINT:						return { 1,			1,			1,			1 };
	case Format::R8_SNORM:						return { 1,			1,			1,			1 };
	case Format::DEPTH_24_UNORM_STENCIL_8_UINT:	return { 1,			4,			1,			4 };
	case Format::BC1:							return { 0,			0,			4,			8 };
	case Format::BC3:							return { 0,			0,			4,			16 };
	}
	return { 0, 0, 1, 0 };
}

void RenderInterface::GetMipDimmensions( const int width, const int height, const int depth, const int mipLevel, TextureDimmensions& result ) {
	const int denominator = Math::Pow2( mipLevel );
	result.width = Math::Max( 1, width / denominator );
	result.height = Math::Max( 1, height / denominator );
	result.depth = Math::Max( 1, depth / denominator );
}

// DeviceObject 

DeviceObject::DeviceObject() {
	references = 1;
}

DeviceObject::~DeviceObject() {}

void DeviceObject::Release() {
	references -= 1;
	if ( references <= 0 ) {
		delete this;
	}
}

void DeviceObject::AddRef() {
	references += 1;
}