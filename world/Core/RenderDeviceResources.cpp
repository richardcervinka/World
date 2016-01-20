#pragma once

#include "RenderDeviceResources.h"

// Buffer

Buffer::Buffer( std::shared_ptr< RenderInterface::Buffer > buffer ) {
	this->buffer = buffer;
}

Buffer::~Buffer() {}

void Buffer::Release() {
	buffer = nullptr;
}

RenderInterface::Buffer* Buffer::GetBuffer() {
	return buffer.get();
}

// VertexBuffer

VertexBuffer::VertexBuffer( std::shared_ptr< RenderInterface::Buffer > buffer, const int capacity ): Buffer( buffer ) {
	this->capacity = capacity;
}

void VertexBuffer::Release() {
	Buffer::Release();
	capacity = nullptr;
}

int VertexBuffer::GetCapacity() const {
	return capacity;
}

// IndexBuffer

IndexBuffer::IndexBuffer( std::shared_ptr< RenderInterface::Buffer > buffer, const int capacity ): Buffer( buffer ) {
	this->capacity = capacity;
}

void IndexBuffer::Release() {
	Buffer::Release();
	capacity = nullptr;
}

int IndexBuffer::GetCapacity() const {
	return capacity;
}

// RenderDeviceResources

RenderDeviceResources::RenderDeviceResources() {
	memoryUsage = 0;
}

VertexBuffer RenderDeviceResources::CreateVertexBuffer(
	const int vertexSize,
	const int capacity,
	const RenderInterface::BufferUsage usage,
	const RenderInterface::BufferAccess access,
	const void* const initialData
) {
	RenderInterface::PBuffer buffer = device->CreateVertexBuffer( vertexSize * capacity, usage, access, initialData );
	if ( !buffer ) {
		return VertexBuffer( nullptr, 0 );
	}
	std::shared_ptr< RenderInterface::Buffer > sharedBuffer( buffer );
	StoreBuffer( sharedBuffer, vertexSize * capacity );
	return VertexBuffer( sharedBuffer, capacity );
}

IndexBuffer RenderDeviceResources::CreateIndexBuffer( const RenderInterface::Format format, const int capacity, const void* const initialData ) {
	int byteWidth = 0;
	if ( format == RenderInterface::Format::R16_UINT ) {
		byteWidth = 2 * capacity;
	}
	if ( format == RenderInterface::Format::R32_UINT ) {
		byteWidth = 4 * capacity;
	}
	RenderInterface::PBuffer buffer = device->CreateIndexBuffer(
		byteWidth,
		RenderInterface::BufferUsage::STATIC,
		RenderInterface::BufferAccess::NONE,
		initialData
	);
	if ( !buffer ) {
		return VertexBuffer( nullptr, 0 );
	}
	std::shared_ptr< RenderInterface::Buffer > sharedBuffer( buffer );
	StoreBuffer( sharedBuffer, byteWidth );
	return IndexBuffer( sharedBuffer, capacity );
}

void RenderDeviceResources::StoreBuffer( std::shared_ptr< RenderInterface::Buffer > buffer, const int byteWidth ) {
	if ( buffers.capacity() <= buffers.size() ) {
		buffers.reserve( buffers.capacity() + 100 );
	}
	buffers.push_back( sharedBuffer );
	memoryUsage += static_cast< unsigned long >( byteWidth );
}

unsigned long RenderDeviceResources::GetMemoryUsage() const {
	return memoryUsage;
}

void RenderDeviceResources::ReleaseUnusedResources() {
	// uvolnit nepouzivane buffery
	int unused = 0;
	for ( auto& buffer : buffers ) {
		if ( buffer.unique() ) {
			buffer = nullptr;
			unused += 1;
		}
	}
	if ( unused == 0 ) {
		return;
	}
	// odstranit z buffers nulove buffery
	decltype( buffers ) usedBuffers;
	usedBuffers.reserve( buffers.size() - unused + 100 );
	for ( auto& buffer : buffers ) {
		if ( buffer ) {
			usedBuffers.push_back( buffer );
		}
	}
	buffers = std::move( usedBuffers );
}

void RenderDeviceResources::ReleaseUnusedResources() {
	buffers.clear();
	buffers.shrink_to_fit();
}