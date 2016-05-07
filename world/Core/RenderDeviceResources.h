#pragma once

#include  <vector>
#include "RenderInterface.h"

/*
Zakladni trida pro buffery
*/
class Buffer {
public:
	Buffer() = default;
	virtual ~Buffer() = 0;

	// Uvolneni bufferu
	virtual void Release();

	// Pouze pro interni potreby Renderer tridy
	RenderInterface::Buffer* GetBuffer();

protected:
	std::shared_ptr< RenderInterface::Buffer > buffer;
};

/*
Vertex buffer
*/
template< typename T >
class TVertexBuffer: public Buffer {
public:
	// sub buffer
	// TVertexBuffer( TVertexBuffer& buffer, const int offset, const int count )

	void Create( std::shared_ptr< RenderInterface::Buffer > buffer ) {
		this->buffer = buffer;
	}

	int Capacity() const {
		return buffer->GetByteWidth() / sizeof( T );
	}

	//int GetVertexByteWidth() const;
};

/*
Index buffer (uint16_t)
*/
class IndexBuffer: public Buffer {
public:
	void Create( std::shared_ptr< RenderInterface::Buffer > buffer );
	int Capacity() const;

private:
};

/*
2D Texture buffer 
*/
class Texture2D: public Buffer {
public:
	//Texture2D( std::shared_ptr< RenderInterface::Buffer > buffer, format, width, height, mipLevels );

private:
};

/*
2D Texture array buffer 
*/
class Texture2DArray: public Buffer {
public:

private:
};

/*
Vytvareni a sprava zdroju graficke karty (textury a buffery).
Objekt uchovava ukazatel na kazdy vytvoreny RenderInterface objekt.
*/
/*
class RenderDeviceResources {
public:
	RenderDeviceResources();
	~RenderDeviceResources();

	// Vytvori vertex buffer, pri selhani vrati vertex buffer s nulovou velikosti
	std::unique_ptr< VertexBuffer > CreateVertexBuffer(
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
*/