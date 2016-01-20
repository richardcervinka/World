#pragma once

#include "RenderInterface.h"

class Buffer {
public:
	Buffer( std::shared_ptr< RenderInterface::Buffer > buffer );
	virtual ~Buffer() = 0;

	// Neni mozne vytvaret kopie
	Buffer( const Buffer& ) = delete;
	Buffer& operator=( const Buffer& ) = delete;

	// Explicitni uvolneni RenderInterface objektu
	virtual void Release();

	// Pouze pro interni potreby Renderer tridy
	RenderInterface::Buffer* GetBuffer();

private:
	std::shared_ptr< RenderInterface::Buffer > buffer;
};

class VertexBuffer: public Buffer {
public:
	VertexBuffer( std::shared_ptr< RenderInterface::Buffer > buffer, const int capacity );

	virtual void Release() override;

	// Maximalni pocet vertexu v bufferu
	int GetCapacity() const;

private:
	int capacity;
};

class IndexBuffer {
public:
	IndexBuffer( std::shared_ptr< RenderInterface::Buffer > buffer, const int capacity );

	virtual void Release() override;

	// Maximalni pocet indexu v bufferu
	int GetCapacity() const;

private:
	int capacity;
};

class Texture2D: public Buffer {
public:
	Texture2D( std::shared_ptr< RenderInterface::Buffer > buffer, format, width, height, mipLevels );

private:
};

class Texture2DArray: public Buffer {
public:

private:
};

/*
Vytvareni a sprava zdroju graficke karty (textury a buffery).
Objekt uchovava ukazatel na kazdy vytvoreny RenderInterface objekt.
*/
class RenderDeviceResources {
public:
	RenderDeviceResources();
	~RenderDeviceResources();

	// Vytvori vertex buffer, pri selhani vrati vertex buffer s nulovou velikosti
	VertexBuffer CreateVertexBuffer(
		const int vertexSize,
		const int capacity,
		const RenderInterface::BufferUsage usage,
		const RenderInterface::BufferAccess access,
		const void* const initialData
	);

	// Vytvori immutable index buffer (obsah bufferu nelze po vytvoreni zmenit, initialData proto nesmi byt nullptr)
	IndexBuffer CreateIndexBuffer( const RenderInterface::Format format, const int capacity, const void* const initialData );

	// Vrati mnozstvi obsazene video pameti
	unsigned long GetMemoryUsage() const;

	// Uvolni vsechny nepouzivane RenderInterface objekty (buffery)
	void ReleaseUnusedResources();

	// Realne jsou zdroje uvolneny az jsou uvolneny vsechny reference (viz std::shared_ptr)
	// void Release();

	// CreateTextureView();
	// CreateIndexBuffer();
	// CreateTextureBuffer();
	// GetVertexBuffer();
	// GetIndexBuffer();
	// GetTextureBuffer();
	// ReleaseVertexBuffer();
	// ReleaseIndexBuffer();
	// ReleaseTextureBuffer();
	// Release();
	// GetMemory();
	// GetTextureIno();
	// UpdateTextureBuffer();
	// UpdateVertexBuffer();
	// UpdateIndexBuffer();
	
private:
	void StoreBuffer( std::shared_ptr< RenderInterface::Buffer > buffer, const int byteWidth );

private:
	std::shared_ptr< RenderInterface::Device > device;
	std::vector< std::shared_ptr< RenderInterface::Buffer > > buffers;
	unsigned long memoryUsage;
};