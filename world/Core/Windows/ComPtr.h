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

	ComPtr( const ComPtr& cp ) {
		ptr = cp.GetRef();
	}
	
	explicit ComPtr( T* const ptr ) {
		if ( ptr != nullptr ) {
			ptr->AddRef();
		}
		Release();
		this->ptr = ptr;
	}

	ComPtr& operator=( T* const ptr ) {
		if ( ptr != nullptr ) {
			ptr->AddRef();
		}
		Release();
		this->ptr = ptr;
		return *this;
	}

	ComPtr& operator=( const nullptr_t ptr ) {
		Release();
		return *this;
	}
	
	ComPtr& operator=( const ComPtr& cp ) {
		T* const cpptr = cp.GetRef();
		Release();
		ptr = cpptr;
		return *this;
	}

	bool operator==( const ComPtr& cptr ) const noexcept {
		return this->ptr == cptr.ptr;
	}

	bool operator!=( const ComPtr& cptr ) const noexcept {
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