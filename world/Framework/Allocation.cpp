#include "Allocation.h"
#include "String.h"
#include "..\Platform\Application.h"

// 16 byte aligned heap allocations

// windows allocation
#ifdef _WIN32
#define ALLOC_16

inline void *alloc_16( const std::size_t size ) {
	return _aligned_malloc( size, 16 );
}

inline void free_16( void * const ptr ) {
	_aligned_free( ptr );
}

#endif // _WIN32

// linux allocation
#ifdef _LINUX
#define ALLOC_16

inline void *alloc_16( const std::size_t size ) {
	void *storage = nullptr;
	posix_memalign( &storage, 16, size );
	return storage;
}

inline void free_16( void * const ptr ) {
	free( ptr );
}

#endif // _LINUX

// custom allocation
#ifndef ALLOC_16
#define ALLOC_16

inline void *alloc_16( const std::size_t size ) {
	// storage = [ offset 0-15 bytes ][ void *storage ][ aligned returned storage ][ 15 minus offset bytes ]
	void *storage = malloc( size + sizeof( void* ) + 15 );
	if ( storage == nullptr ) {
		return nullptr;
	}
	// vypocitat zacatek aligned bloku
	char *aligned = reinterpret_cast< char* >( storage ) + sizeof( void* );
	aligned += ( 16 - ( reinterpret_cast< std::size_t >( aligned ) % 16 ) ) & 0x0f;
	
	// vlozit pred aligned blok ukazatel na storage (pro potreby operator delete)
	*reinterpret_cast< void** >( aligned - sizeof( char* ) ) = storage;
	
	return aligned;
}

inline void free_16( void * const ptr ) {
	char *storage = reinterpret_cast< char* >( ptr ) - sizeof( char* );
	free( *reinterpret_cast< char** >( storage ) );
}

#endif // ALLOC_16

inline void *Alloc( const std::size_t size ) {
	std::size_t bytes = size;
	if ( bytes == 0 ) {
		bytes = 1;
	}
	void *storage = alloc_16( bytes );
	while ( storage == nullptr ) {
		std::new_handler handler = std::get_new_handler();
		if ( handler == nullptr ) {
			return nullptr;
		}
		handler();
		storage = alloc_16( bytes );
	}
	return storage;
}

void *operator new( std::size_t size ) {
	return Alloc( size );
}

void *operator new[]( std::size_t size ) {
	return Alloc( size );
}

void operator delete( void *ptr ) noexcept {
	if ( ptr != nullptr ) {
		free_16( ptr );
	}
}

void operator delete[]( void *ptr ) noexcept {
	if ( ptr != nullptr ) {
		free_16( ptr );
	}
}

// FixedAllocator

FixedAllocator::FixedAllocator( const unsigned long size, const unsigned long chunkSize ) {
	allocated = 0;
	chunks = nullptr;
	free = nullptr;
	this->size = ( size + 15 ) & ( ~0x0f ); // 16 byte alignment
	this->chunkSize = chunkSize;
}

FixedAllocator::~FixedAllocator() {
	// Chyba, je alokovana nejaka pamet!
	if ( allocated > 0 ) {
		Application::Abort( String( u"FixedAllocator destruction failed, there is still allocated memory!" ) );
	}
	Chunk *chunk = chunks;
	while ( chunk != nullptr ) {
		Chunk *next = chunk->next;
		delete [] chunk;
		chunk = next;
	}
}

void FixedAllocator::Reserve( const unsigned long bytes ) {
	Expand( ( bytes / size ) + 1 );
}

void FixedAllocator::Expand( const unsigned long count ) {
	// alokovat pamet
	Byte *storage = new Byte[ sizeof( Chunk ) + count * ( sizeof( Block ) + size ) ];
	
	// inicializovat chunk
	Chunk *chunk = reinterpret_cast< Chunk* >( storage );
	chunk->allocated = 0;
	chunk->count = count;
	
	// vlozit novy chunk na vrchol seznamu
	chunk->next = chunks;
	chunks = chunk;
	
	// inicializovat bloky
	unsigned long blockWidth = sizeof( Block ) + size;
	Byte *ptr = storage + sizeof( Chunk );
	for ( unsigned long i = 0; i < count - 1; i++ ) {
		reinterpret_cast< Block* >( ptr )->chunk = chunk;
		reinterpret_cast< Block* >( ptr )->next = reinterpret_cast< Block* >( ptr + blockWidth );
		ptr += blockWidth;
	}
	reinterpret_cast< Block* >( ptr )->chunk = chunk;
	reinterpret_cast< Block* >( ptr )->next = nullptr;
	
	// nastavi free prvni blok
	free = reinterpret_cast< Block* >( storage + sizeof( Chunk ) );
}

void *FixedAllocator::Alloc() {
	if ( free == nullptr ) {
		Expand( chunkSize );
	}
	// nepodarilo se volani funkce Expand()
	if ( free == nullptr ) {
		return nullptr;
	}
	Block *block = free;
	free = block->next;
	block->chunk->allocated += 1;
	allocated += 1;
	return reinterpret_cast< Byte* >( block ) + sizeof( Block );
}

void FixedAllocator::Free( void * const ptr ) {
	if ( ptr == nullptr ) {
		return;
	}
	Block *block = reinterpret_cast< Block* >( reinterpret_cast< Byte* >( ptr ) - sizeof( Block ) );
	block->next = free;
	free = block;
	block->chunk->allocated -= 1;
	allocated -= 1;
}

void FixedAllocator::Flush() {
	Chunk *chunk = chunks;
	while ( chunk != nullptr ) {
		if ( chunk->allocated > 0 ) {
			chunk = chunk->next;
			continue;
		}
		if ( chunks == chunk ) {
			chunks = chunk->next;
		}
		Chunk *empty = chunk;
		chunk = chunk->next;
		delete [] empty;
	}
}

unsigned long FixedAllocator::Allocated() const {
	return allocated;
}

unsigned long FixedAllocator::MemoryOccupied() const {
	unsigned long bytes = 0;
	Chunk *chunk = chunks;
	while ( chunk != nullptr ) {
		bytes += sizeof( Chunk ) + chunk->count * ( sizeof( Block ) + size );
	}
	return bytes;
}

unsigned long FixedAllocator::MemoryAllocated() const {
	return allocated * size;
}