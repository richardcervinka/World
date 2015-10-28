#include "RenderInterface.h"
#include "DX11\DX11RenderInterface.h"

using namespace RenderInterface;

Device *RenderInterface::DX11CreateDevice( const DX11CreateDeviceParams &params ) {
	DX11Device *device = new DX11Device();
	if ( device == nullptr ) {
		return nullptr;
	}
	if ( !device->Create( params ) ) {
		delete device;
		return nullptr;
	}
	return device;
}

const FormatDesc RenderInterface::GetFormatDesc( const Format &format ) {
	static const FormatDesc table[] = {					
		{ Format::UNKNOWN,              0, 0,  0 },
		{ Format::R8G8B8A8_UNORM,       4, 8,  4 },
		{ Format::R8G8B8A8_SNORM,       4, 8,  4 },
		{ Format::R16G16B16A16_FLOAT,   4, 16, 8 },
		{ Format::R16G16_FLOAT,         2, 16, 4 },
		{ Format::R8_UNORM,             1, 8,  1 },
		{ Format::R16_FLOAT,            1, 16, 2 },
		{ Format::R32_FLOAT,            1, 32, 4 },
		{ Format::BC1,                  1, 8,  2 },
		{ Format::BC3,                  1, 16, 4 }
	};
	return table[ static_cast< unsigned int >( format ) ];
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