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
	switch ( format ) {																			//channelCount	channelBytes	blockSize		blockByteWidth
	case Format::UNKNOWN:						return { Format::UNKNOWN,						0,				0,				0,				0 };
	case Format::R8G8B8A8_UNORM:				return { Format::R8G8B8A8_UNORM,				4,				1,				1,				4 };
	case Format::R8G8B8A8_SNORM:				return { Format::R8G8B8A8_SNORM,				4,				1,				1,				4 };
	case Format::R16G16B16A16_FLOAT:			return { Format::R16G16B16A16_FLOAT,			4,				2,				1,				8 };
	case Format::R16G16_FLOAT:					return { Format::R16G16_FLOAT,					2,				2,				1,				4 };
	case Format::R8_UNORM:						return { Format::R8_UNORM,						1,				1,				1,				1 };
	case Format::R16_FLOAT:						return { Format::R16_FLOAT,						1,				2,				1,				2 };
	case Format::R32_FLOAT:						return { Format::R32_FLOAT,						1,				4,				1,				4 };
	case Format::DEPTH_24_UNORM_STENCIL_8_UINT:	return { Format::DEPTH_24_UNORM_STENCIL_8_UINT,	0,				0,				1,				4 };
	case Format::BC1:							return { Format::BC1,							0,				0,				4,				8 };
	case Format::BC3:							return { Format::BC3,							0,				0,				4,				16 };
	}
	return { Format::UNKNOWN, 0, 0, 0, 0 };
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