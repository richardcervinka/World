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
		{ Format::UNKNOWN,                          0, 0,  0 },
		{ Format::R8G8B8A8_UNORM,                   4, 8,  4 },
		{ Format::R8G8B8A8_SNORM,                   4, 8,  4 },
		{ Format::R16G16B16A16_FLOAT,               4, 16, 8 },
		{ Format::R16G16_FLOAT,                     2, 16, 4 },
		{ Format::R8_UNORM,                         1, 8,  1 },
		{ Format::R16_FLOAT,                        1, 16, 2 },
		{ Format::R32_FLOAT,                        1, 32, 4 },
		{ Format::DEPTH_24_UNORM_STENCIL_8_UINT,    1, 32, 4 },
		{ Format::BC1,                              1, 8,  2 },
		{ Format::BC3,                              1, 16, 4 }
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

// TextureBuffer

TextureBuffer::TextureBuffer() {
	ZeroMemory( &desc, sizeof( desc ) );
}

const Format TextureBuffer::GetFormat() const {
	return desc.format;
}

TextureBufferType TextureBuffer::GetType() const {
	return desc.type;
}
		
int TextureBuffer::GetDimmension() const {
	static const int table[] = {
		0,	// UNDEFINED
		1,	// TEXTURE_1D
		1,	// TEXTURE_1D_ARRAY
		2,	// TEXTURE_2D
		2,	// TEXTURE_2D_ARRAY
		2,	// TEXTURE_2D_MS,
		2,	// TEXTURE_2D_MS_ARRAY,
		3	// TEXTURE_3D
	};
	return table[ static_cast< int >( desc.type ) ];
}
		
TextureDimmensions TextureBuffer::GetDimmensions() const {
	return desc.dimmensions;
}
		
int TextureBuffer::GetArraySize() const {
	return desc.arraySize;
}
		
int TextureBuffer::GetSamplesCount() const {
	return desc.samplesCount;
}
		
int TextureBuffer::GetSamplesQuality() const {
	return desc.samplesQuality;
}

bool TextureBuffer::RenderTargetUsable() const {
	return desc.renderTarget;
}

void TextureBuffer::SetDesc( const TextureBufferDesc &desc ) {
	this->desc = desc;
}