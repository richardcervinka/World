#pragma once

#include <new>
#include <cstdlib>

// 16 byte aligned allocations
void* operator new( std::size_t size );
void* operator new[]( std::size_t size );
void operator delete( void *ptr ) noexcept;
void operator delete[]( void *ptr ) noexcept;

const unsigned long DEFAULT_ALLOCATOR_CHUNK_SIZE = 128;

// Rozhrani obecneho alokatoru
// Allocatory pouzivaji typ unsigned long, vetsinou odpovida typu std::size_t, tedy i sizeof()
//
class Allocator {
public:
	Allocator() {}
	virtual ~Allocator() {}
	
	// neni povoleno vytvaret kopie
	Allocator( const Allocator& ) = delete;
	Allocator &operator=( const Allocator& ) = delete;
	
	// alokuje nejmene bytes bajtu systemove pameti
	virtual void Reserve( const unsigned long bytes ) = 0;
	
	// vrati pocet alokovanych bloku pameti (tj. pocet volani funkci typu Alloc() ) 
	virtual unsigned long Allocated() const = 0;
	
	// uvolni nejvetsi mozne mnozstvi systemove pameti
	virtual void Flush() = 0;
	
	// Vrati pocet bajtu systemove pameti obsazene alokatorem, vcetne hlavicek bloku...
	// Nezahrnuje pamet obsazenou samotnym objektem Allocator
	virtual unsigned long MemoryOccupied() const = 0;
	
	// vrati pocet bajtu pameti poskytnute alokatorem, tj. pameti primo vyuzitelne klientem tridy Allocator
	virtual unsigned long MemoryAllocated() const = 0;
};

// FixedAllocator Alokuje bloky pameti stejne velikosti.
// Alokovany blok pameti je vzdy 16 byte aligned.
//
class FixedAllocator: public Allocator {
public:
	explicit FixedAllocator( const unsigned long size, const unsigned long chunkSize = DEFAULT_ALLOCATOR_CHUNK_SIZE );
	~FixedAllocator();
	
	// vrati ukazatel na blok pameti
	void *Alloc();
	
	// uvolni blok pameti
	void Free( void * const ptr );
	
	// implementace rozhrani tridy Allocator
	virtual void Reserve( const unsigned long bytes );
	virtual unsigned long Allocated() const;
	virtual void Flush();
	virtual unsigned long MemoryOccupied() const;
	virtual unsigned long MemoryAllocated() const;
	
private:
	struct alignas( 16 ) Chunk {
		Chunk *next;
		unsigned long count;		// pocet bloku v chunku
		unsigned long allocated;	// pocet alokovanych bloku
	};
	
	struct alignas( 16 ) Block {
		Block *next;
		Chunk *chunk;
	};
	
	// alokuje systemovou pamet pro count bloku
	void Expand( const unsigned long count );
	
private:
	Chunk *chunks;
	Block *free;
	unsigned long size;
	unsigned long chunkSize;
	unsigned long allocated;
};

// Vrati ukazatel na novou instanci objektu typu T, ekvivalent operatoru new a delete
//
template <typename T>
class ObjectAllocator: public Allocator {
public:
	explicit ObjectAllocator( const unsigned long chunkSize = DEFAULT_ALLOCATOR_CHUNK_SIZE );
	
	T *New();
	void Delete( T * const ptr );
	
	// implementace rozhrani tridy Allocator
	virtual void Flush();
	virtual void Reserve( const unsigned long bytes );
	virtual unsigned long Allocated() const;
	virtual unsigned long MemoryOccupied() const;
	virtual unsigned long MemoryAllocated() const;
	
private:
	FixedAllocator allocator;
};

template <typename T>
ObjectAllocator<T>::ObjectAllocator( const unsigned long chunkSize ):
	allocator( sizeof( T ), chunkSize )
{
	// vsechny members jsou inicializovany v member initializer list
}

template <typename T>
inline T *ObjectAllocator<T>::New() {
	return new ( allocator.Alloc() ) T();
}

template <typename T>
inline void ObjectAllocator<T>::Delete( T * const ptr ) {
	if ( ptr == nullptr ) {
		return;
	}
	ptr->~T();
	allocator.Free( ptr );
}

template <typename T>
inline void ObjectAllocator<T>::Reserve( const unsigned long bytes ) {
	allocator.Reserve( bytes );
}

template <typename T>
inline unsigned long ObjectAllocator<T>::Allocated() const {
	return allocator.Allocated();
}

template <typename T>
inline void ObjectAllocator<T>::Flush() {
	allocator.Flush();
}

template <typename T>
inline unsigned long ObjectAllocator<T>::MemoryOccupied() const {
	return allocator.MemoryOccupied();
}

template <typename T>
inline unsigned long ObjectAllocator<T>::MemoryAllocated() const {
	return allocator.MemoryAllocated();
}