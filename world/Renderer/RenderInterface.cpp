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

const FormatDesc RenderInterface::GetFormatDesc( const Format format ) {
	switch ( format ) {
	case Format::UNKNOWN:						return { Format::UNKNOWN,						0, 0,  0 };
	case Format::R8G8B8A8_UNORM:				return { Format::R8G8B8A8_UNORM,				4, 8,  4 };
	case Format::R8G8B8A8_SNORM:				return { Format::R8G8B8A8_SNORM,				4, 8,  4 };
	case Format::R16G16B16A16_FLOAT:			return { Format::R16G16B16A16_FLOAT,			4, 16, 8 };
	case Format::R16G16_FLOAT:					return { Format::R16G16_FLOAT,					2, 16, 4 };
	case Format::R8_UNORM:						return { Format::R8_UNORM,						1, 8,  1 };
	case Format::R16_FLOAT:						return { Format::R16_FLOAT,						1, 16, 2 };
	case Format::R32_FLOAT:						return { Format::R32_FLOAT,						1, 32, 4 };
	case Format::DEPTH_24_UNORM_STENCIL_8_UINT:	return { Format::DEPTH_24_UNORM_STENCIL_8_UINT,	1, 32, 4 };
	case Format::BC1:							return { Format::BC1,							1, 8,  2 };
	case Format::BC3:							return { Format::BC3,							1, 16, 4 };
	}
	return { Format::UNKNOWN, 0, 0, 0 };
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
	switch ( desc.type ) {
	case TextureBufferType::UNDEFINED:				return 0;
	case TextureBufferType::TEXTURE_1D:				return 1;
	case TextureBufferType::TEXTURE_1D_ARRAY:		return 1;
	case TextureBufferType::TEXTURE_2D:				return 2;
	case TextureBufferType::TEXTURE_2D_ARRAY:		return 2;
	case TextureBufferType::TEXTURE_2D_MS:			return 2;
	case TextureBufferType::TEXTURE_2D_MS_ARRAY:	return 2;
	case TextureBufferType::TEXTURE_3D:				return 3;
	}
	return 0;
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

void TextureBuffer::SetDesc( const TextureBufferDesc& desc ) {
	this->desc = desc;
}