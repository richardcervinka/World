#pragma once

/*
COM smart pointer.
*/
template< typename T >
class ComPtr {
public:
	ComPtr() {
		ptr = nullptr;
	}
	
	~ComPtr() {
		Release();
	}

	template< typename P >
	ComPtr( const ComPtr< P >& cp ) {
		ptr = cp.GetRef();
	}

	template< typename P >
	ComPtr( P* const ptr ) {
		ptr->AddRef();
		Release();
		this->ptr = ptr;
	}

	ComPtr& operator=( const nullptr_t ptr ) {
		Release();
		return *this;
	}
	
	template< typename P >
	ComPtr& operator=( const ComPtr< P >& cp ) {
		T* cpptr = cp.GetRef();
		Release();
		ptr = cpptr;
		return *this;
	}

	bool operator==( const ComPtr< T >& cptr ) const noexcept {
		return this->ptr == cptr.ptr;
	}

	bool operator!=( const ComPtr< T >& cptr ) const noexcept {
		return this->ptr != cptr.ptr;
	}

	bool operator==( const T* const ptr ) const noexcept {
		return this->ptr == ptr;
	}

	bool operator!=( const T* const ptr ) const noexcept {
		return this->ptr != ptr;
	}

	T* operator->() const noexcept {
		return ptr;
	}
	
	T** operator&() noexcept {
		return &ptr;
	}

	T* Raw() const noexcept {
		return ptr;
	}

	T* GetRef() const noexcept {
		if ( ptr != nullptr ) {
			ptr->AddRef();
		}
		return ptr;
	}

	void Release() noexcept {
		if ( ptr != nullptr ) {
			ptr->Release();
			ptr = nullptr;
		}
	}

private:
	T* ptr;
};