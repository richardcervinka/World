#pragma once

#include <initializer_list>

// Dynamicke pole
//
template <typename T>
class Array {
public:
	Array();
	Array( const Array &arr );
	Array &operator=( const Array &arr );
	~Array() noexcept;
	
	// vytvori pole o velikosti size, prvky nejsou inicializovany
	explicit Array( const int size );
	
	// vytvori pole o velikosti size, prvky inicializuje na hodnotu initializer
	Array( const int size, const T &initializer );
	
	// vytvori sub array
	Array( const Array &arr, const int index, const int count );
	
	// podpora C++11 initialization list
	Array( std::initializer_list<T> initlist );
	
	// podpora c++11 Range-based for loops
	const T *begin() const;
	const T *end() const;
	T *begin();
	T *end();
	
	// operatory pristupu neprovadi kontrolu rozsahu, pro bezpecny pristup slouzi funkce typu At()
	T &operator[]( const int index );
	const T &operator[]( const int index ) const;
	
	// porovnani poli
	bool operator==( const Array &arr ) const;
	bool operator!=( const Array &arr ) const;
	
	// Vrati ukazatel na pole pole typu T*, pouzivat jen ve specialnich pripadech.
	// Operace s polem muzou zpusobit, ze se ukazatel stane neplatnym.
	const T *Raw() const;
	
	// Rozsiri kapacitu minimalne o hodnotu size. Pokud je size <= 0, nedela nic.
	void Reserve( const int size );
	
	// Uvolni vetsinu nevyuzite pameti
	void Flush();
	
	// Zmeni velikost pole. Pokud je velikost mensi nez soucasna velikost, pole se orizne!
	// Prvky pole nejsou pri zvetseni inicializovany.
	void Resize( const int size );

	// Zmeni velikost pole. Pokud je velikost mensi nez soucasna velikost, pole se orizne!
	// Prvky pole jsou pri zvetseni inicializovany hodnotou initializer.
	void Resize( const int size, const T &initializer );
	
	// vlozi prvek na konec pole, velikost pole se zversi o 1. Vraci novou velikost pole.
	int Push( const T &item );
	
	// odstrani prvek z konce pole, velikost pole se zmensi o 1. Vraci index vyjmuteho prvku
	int Pop();
	
	// Zkrati pole na nulovou delku a uvolni veskerou pamet
	void Clear();
	
	// Aktualni pocet prvku v poli
	int Length() const;
	
	// Pocet prvku, ktere muze pole obsahovat nez dojde k alokaci pameti
	int Capacity() const;
	
	// Vyhledavani v poli, -1 pokud neni prvek nalezen. K porovnani pouziva operator ==
	int Find( const int start, const T &ref ) const;
	
	// Vraci true, pokud je v poli obsazen prvek ref
	bool Contains( const T &ref ) const;
	
	// Prehodi pozadi prvku v poli
	void Reverse();
	
	// Seradi prvky pole, k porovnani pouziva callback funkci sort()
	void Sort( void ( *cmpr )( const T *left, const T *right ) );
	
	// Seradi vzestupne prvky pole, k porovnani pouziva operator >=
	//void SortUp();
	
	// Seradi sestupne prvky pole, k porovnani pouziva operator <=
	//void SortDown();
	
private:
	// provadi hloubkovou kopii pole arr. Neuvolnuje zadnou pamet!
	void Copy( const Array &arr );

private:
	enum { granularity = 128 };
	int length;
	int capacity;
	T *data;
};

template <typename T>
Array<T>::Array() {
	length = 0;
	capacity = 0;
	data = nullptr;
}

template <typename T>
Array<T>::Array( const Array<T> &arr ) {
	Copy( arr );
}

template <typename T>
Array<T>::Array( const int size ): Array() {
	Resize( size );
}

template <typename T>
Array<T>::Array( const int size, const T &initializer ): Array() {
	Resize( size, initializer );
}

template <typename T>
Array<T>::Array( const Array &arr, const int index, const int count ): Array() {
	// neplatny rozsah, ponechat prazdne pole
	if ( index + count > arr.length ) {
		return;
	}
	// zkopirovat prvky v rozsahu <index; index + count)
	Resize( count );
	int isrc = index;
	for ( int idest = 0; idest < count; idest++ ) {
		data[ idest ] = arr.data[ isrc++ ];
	}
}

template <typename T>
Array<T>::Array( std::initializer_list<T> initlist ): Array() {
	Resize( initlist.size() );
	int index = 0;
	for ( const T &item : initlist ) {
		data[ index++ ] = item;
	}
}

template <typename T>
Array<T> &Array<T>::operator=( const Array<T> &arr ) {
	if ( &arr == this ) {
		return *this;
	}
	T *old = data;
	Copy( arr );
	delete [] old;
	return *this;
}

template <typename T>
Array<T>::~Array() noexcept {
	delete [] data;
}

template <typename T>
void Array<T>::Copy( const Array<T> &arr ) {
	data = new T[ arr.capacity ];
	length = arr.length;
	capacity = arr.capacity;
	for ( int i = 0; i < length; i++ ) {
		data[ i ] = arr.data[ i ];
	}
}

template <typename T>
inline const T *Array<T>::begin() const {
	return data;
}

template <typename T>
inline T *Array<T>::begin() {
	return data;
}

template <typename T>
inline const T *Array<T>::end() const {
	return data + length;
}

template <typename T>
inline T *Array<T>::end() {
	return data + length;
}

template <typename T>
T &Array<T>::operator[]( int index ) {
	return data[ index ];
}

template <typename T>
inline const T &Array<T>::operator[]( const int index ) const {
	return data[ index ];
}

template <typename T>
const T *Array<T>::Raw() const {
	return data;
}

template <typename T>
inline int Array<T>::Length() const {
	return length;
}

template <typename T>
inline int Array<T>::Capacity() const {
	return capacity;
}

template <typename T>
void Array<T>::Reserve( const int size ) {
	if ( size <= 0 ) {
		return;
	}
	const int newCapacity = capacity + ( ( size + granularity ) / granularity ) * granularity;
	T *newData = new T[ newCapacity ];
	capacity = newCapacity;
	for ( int i = 0; i < length; i++ ) {
		newData[ i ] = data[ i ];
	}
	delete [] data;
	data = newData;
}

template <typename T>
void Array<T>::Flush() {
	int newCapacity = ( ( length + granularity ) / granularity ) * granularity;
	if ( newCapacity == capacity ) {
		return;
	}
	T *newData = new T[ newCapacity ];
	capacity = newCapacity;
	for ( int i = 0; i < length; i++ ) {
		newData[ i ] = data[ i ];
	}
	delete [] data;
	data = newData;
}

template <typename T>
void Array<T>::Resize( const int size ) {
	Reserve( size - capacity );
	length = size;
}

template <typename T>
void Array<T>::Resize( const int size, const T &initializer ) {
	Reserve( size - capacity );
	for ( int i = length; i < size; i++ ) {
		data[ i ] = initializer;
	}
	length = size;
}

template <typename T>
int Array<T>::Push( const T &item ) {
	if ( length == capacity ) {
		Reserve( 1 );
	}
	data[ length ] = item;
	length += 1;
	return length;
}

template <typename T>
int Array<T>::Pop() {
	if ( length == 0 ) {
		return 0;
	}
	length -= 1;
	return length;
}

template <typename T>
void Array<T>::Clear() {
	delete [] data;
	data = nullptr;
	capacity = 0;
	length = 0;
}

template <typename T>
bool Array<T>::operator==( const Array<T> &arr ) const {
	if ( length != arr.length ) {
		return false;
	}
	for ( int i = 0; i < length; i++ ) {
		if ( data[ i ] != arr.data[ i ] ) {
			return false;
		}
	}
	return true;
}

template <typename T>
bool Array<T>::operator!=( const Array<T> &arr ) const {
	return !( *this == arr );
}

template <typename T>
int Array<T>::Find( const int start, const T &ref ) const {
	for ( int i = start; i < length; i++ ) {
		if ( data[ i ] == ref ) {
			return i;
		}
	}
	return -1;
}

template <typename T>
bool Array<T>::Contains( const T &ref ) const {
	return Find( 0, ref ) >= 0;
}

template <typename T>
void Array<T>::Sort( void ( *cmpr )( const T*, const T* ) ) {
	qsort( data, length, sizeof( T ), cmpr );
}

template <typename T>
void Array<T>::Reverse() {
	int left = 0;
	int right = length - 1;
	while ( left < right ) {
		T tmp = data[ left ];
		data[ left ] = data[ right ];
		data[ right ] = tmp;
		left++;
		right--;
	}
}