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
	case Format::UNKNOWN:						return { 0,			0,			0,			0 };
	case Format::R8G8B8A8_UNORM:				return { 4,			1,			1,			4 };
	case Format::R8G8B8A8_SNORM:				return { 4,			1,			1,			4 };
	case Format::R16G16B16A16_FLOAT:			return { 4,			2,			1,			8 };
	case Format::R16G16_FLOAT:					return { 2,			2,			1,			4 };
	case Format::R8_UNORM:						return { 1,			1,			1,			1 };
	case Format::R16_FLOAT:						return { 1,			2,			1,			2 };
	case Format::R16_UINT:						return { 1,			2,			1,			2 };
	case Format::R32_FLOAT:						return { 1,			4,			1,			4 };
	case Format::R32_UINT:						return { 1,			4,			1,			4 };
	case Format::DEPTH_24_UNORM_STENCIL_8_UINT:	return { 0,			0,			1,			4 };
	case Format::BC1:							return { 0,			0,			4,			8 };
	case Format::BC3:							return { 0,			0,			4,			16 };
	}
	return { 0, 0, 0, 0 };
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