#include <cwchar>
#include <cstring>
#include <cctype>
#include "String.h"
#include "Math.h"
#include "Types.h"

void WcharToChar16( const wchar_t* const src, char16_t* const dest, const int length ) {
	if ( src == nullptr || dest == nullptr ) {
		return;
	}
	if ( length <= 0 ) {
		dest[ 0 ] = u'\0';
		return;
	}
	const wchar_t* srcChar = src;
	char16_t* destChar = dest;

	for ( int i = 0; i < length; i++ ) {
		if ( *src >= 0xD800 && *src <= 0xDFFF ) {
			*destChar = u' ';
		} else {
			*destChar = static_cast< char16_t >( *srcChar );
		}
		srcChar += 1;
		destChar += 1;
	}
}

// String

String::String() {
	length = 0;
	capacity = SHORT_LENGTH;
	stringLong = nullptr;
	string = stringShort;
	string[ 0 ] = u'\0';
}

String::String( const char16_t * const strUTF16 ): String() {
	FromUTF16( strUTF16 );
}

String::String( const String &str ): String() {
	Alloc( str.capacity );
	length = str.length;
	memcpy( string, str.string, sizeof( char16_t ) * ( length + 1 ) );
}

String::String( const String &str, const int index, const int size ): String() {
	Set( str, index, size );
}

String::String( String &&str ) {
	Move( str );
}

String &String::operator=( String &&str ) {
	// uvolnit pamet
	delete [] stringLong;
	stringLong = nullptr;
	Move( str );
	return *this;
}

void String::Move( String &str ) {
	// zkopirovat short string
	if ( str.string == str.stringShort ) {
		length = str.length;
		capacity = SHORT_LENGTH;
		stringLong = nullptr;
		string = stringShort;
		memcpy( string, str.string, sizeof( char16_t ) * ( length + 1 ) );
		return;
	}
	// move long string
	length = str.length;
	capacity = str.capacity;
	stringLong = str.stringLong;
	str.stringLong = nullptr;
	string = stringLong;
}

String::~String() {
	delete [] stringLong;
}

String &String::operator=( const String &str ) {
	if ( &str == this ) {
		return *this;
	}
	Realloc( str.capacity );
	length = str.length;
	memcpy( string, str.string, sizeof( char16_t ) * ( length + 1 ) );
	return *this;
}

String &String::operator=( const char16_t * const strUTF16 ) {
	FromUTF16( strUTF16 );
	return *this;
}

void String::Set( const String &str, const int index, const int size ) {
	if ( index < 0 || size < 0 || size > str.length - index ) {
		Clear();
		return;
	}
	Alloc( size + 1 );
	memcpy( string, str.string + index, size * sizeof( char16_t ) );
	length = size;
	string[ length ] = u'\0';
}

void String::FromUCS( const char * const str ) {
	if ( str == nullptr ) {
		return;
	}
	int size = strlen( str );
	Alloc( size + 1 );
	
	for ( int i = 0; i < size; i++ ) {
		string[ i ] = static_cast< char16_t >( str[ i ] );
	}
	string[ size ] = u'\0';
	length = size;
}

void String::FromUCS2( const char16_t * const str ) {
	if ( str == nullptr ) {
		return;
	}
	int size = LengthUCS2( str );
	Alloc( size + 1 );
	memcpy( string, str, 2 * size );
	string[ size ] = u'\0';
	length = size;
}

void String::FromUTF8( const char * const str ) {
	if ( str == nullptr ) {
		return;
	}
	int size = LengthUTF8( str );
	Alloc( size + 1 );
	
	const unsigned char *src = reinterpret_cast< const unsigned char * >( str );
	char16_t *dest = string;
	while ( *src != '\0' ) {
		// 1 byte znak
		if ( *src < 0xc0 ) {
			*dest = static_cast< char16_t >( *src );
			src += 1;
			dest += 1;
			continue;
		}
		// 2 byte znak
		if ( *src < 0xe0 ) {
			unsigned int value =
				( static_cast< char16_t >( *( src + 1 ) ) & 0x003f ) |
				( ( static_cast< char16_t >( *src ) & 0x001f ) << 6 );
				
			*dest = ( value > 0xffff ? 0xffff : static_cast< char16_t >( value ) );
			src += 2;
			dest += 1;
			continue;
		}
		// 3 byte znak
		if ( *src < 0xf0 ) {
			unsigned int value =
				( static_cast< char16_t >( *( src + 2 ) ) & 0x003f ) |
				( ( static_cast< char16_t >( *( src + 1 ) ) & 0x003f ) << 6 ) |
				( ( static_cast< char16_t >( *src ) & 0x000f ) << 12 );
				
			*dest = ( value > 0xffff ? 0xffff : static_cast< char16_t >( value ) );
			src += 3;
			dest += 1;
			continue;
		}
		
		// ostatni vicebajtove znaky jsou vzdy mimo rozsah, ulozit 0xffff
		*dest = 0xffff;
		dest += 1;
		
		// posunout src o spravny pocet bajtu
		if ( *src < 0xf8 ) {
			src += 4;
		} else if ( *src < 0xfc ) {
			src += 5;
		} else {
			src += 6;
		}
	}
	string[ size ] = u'\0';
	length = size;
}

void String::FromUTF16( const char16_t * const str ) {
	if ( str == nullptr ) {
		return;
	}
	int size = LengthUTF16( str );
	Alloc( size + 1 );
	
	const char16_t *src = str;
	char16_t *dest = string;
	while ( *src != u'\0' ) {
		// 2 bajtovy znak
		if ( *src < 0xD800 || *src > 0xDFFF ) {
			*dest = *src;
			src += 1;
			dest += 1;
			continue;
		}
		// 4 bajtovy znak
		*dest = 0xffff;
		src += 2;
		dest += 1;
	}
	string[ size ] = u'\0';
	length = size;
}

void String::Expand( const int size ) {
	if ( size <= capacity ) {
		return;
	}
	char16_t *expanded = new char16_t[ size ];
	
	// zkopirovani puvodniho retezce vcetne znaku 0
	memcpy( expanded, string, 2 * length + 1 );
	
	// alokovat novou pamet
	delete [] stringLong;
	stringLong = expanded;
	string = stringLong;
	capacity = size;
}

void String::Alloc( const int size ) {
	if ( size <= capacity ) {
		string[ 0 ] = u'\0';
		length = 0;
		return;
	}
	char16_t *storage = new char16_t[ size ];
	delete [] stringLong;
	stringLong = storage;
	string = stringLong;
	string[ 0 ] = u'\0';
	length = 0;
	capacity = size;
}

void String::Realloc( const int size ) {
	if ( size > SHORT_LENGTH ) {
		char16_t *storage = new char16_t[ size ];
		delete [] stringLong;
		stringLong = storage;
		string = stringLong;
		capacity = size;
	} else {
		delete [] stringLong;
		stringLong = nullptr;
		string = stringShort;
		capacity = SHORT_LENGTH;
	}
	string[ 0 ] = u'\0';
	length = 0;
}

int String::LengthUTF8( const char * const str ) {
	if ( str == nullptr ) {
		return 0;
	}
	int count = 0;
	const unsigned char *ch = reinterpret_cast< const unsigned char * >( str );
	while ( *ch != '\0' ) {
		count += 1;
		
		// posunout o presny pocet bajtu
		if ( *ch < 0xc0 ) {
			ch += 1;
			continue;
		}
		if ( *ch < 0xe0 ) {
			ch += 2;
			continue;
		}
		if ( *ch < 0xf0 ) {
			ch += 3;
			continue;
		}
		if ( *ch < 0xf8 ) {
			ch += 4;
			continue;
		}
		if ( *ch < 0xfc ) {
			ch += 5;
			continue;
		}
		ch += 6;
	}
	return count;
}

int String::LengthUTF16( const char16_t * const str ) {
	if ( str == nullptr ) {
		return 0;
	}
	int count = 0;
	const char16_t *ch = str;
	while ( *ch != u'\0' ) {
		count += 1;
		
		// 2 bajtovy znak
		if ( *ch < 0xD800 || *ch > 0xDFFF ) {
			ch += 1;
			continue;
		}
		// 4 bajtovy znak
		ch += 2;
	}
	return count;
}

int String::LengthUCS2( const char16_t * const str ) {
	if ( str == nullptr ) {
		return 0;
	}
	int count = 0;
	const char16_t *ch = str;
	while ( *ch != u'\0' ) {
		count += 1;
		ch += 1;
	}
	return count;
}

int String::Compare( const String &right ) const {
	int cmp = memcmp( string, right.string, Math::Min( length * 2, right.length * 2 ) );
	
	if ( length == right.length ) {
		return cmp;
	}
	// pokud se shoduji substringy, tak delsi je vetsi
	if ( cmp == 0 ) {
		return length - right.length;
	}
	return cmp;
}

bool String::operator==( const String &str ) const {
	if ( length != str.length ) {
		return false;
	}
	return memcmp( string, str.string, length * 2 ) == 0;
}

bool String::operator!=( const String &str ) const {
	if ( length != str.length ) {
		return true;
	}
	return memcmp( string, str.string, length * 2 ) != 0;
}

bool String::operator>( const String &str ) const {
	return Compare( str ) > 0;
}

bool String::operator>=( const String &str ) const {
	return Compare( str ) >= 0;
}

bool String::operator<( const String &str ) const {
	return Compare( str ) < 0;
}

bool String::operator<=( const String &str ) const {
	return Compare( str ) <= 0;
}

void String::ToUpper() {
	for ( int i = 0; i < length; i++ ) {
		string[ i ] = static_cast< char16_t >( toupper( string[ i ] ) );
	}
}

void String::ToLower() {
	for ( int i = 0; i < length; i++ ) {
		string[ i ] = static_cast< char16_t >( tolower( string[ i ] ) );
	}
}

void String::Append( const String &str ) {
	if ( str.length <= 0 ) {
		return;
	}
	Expand( length + str.length + 1 );
	memcpy( string + length, str.string, str.length * 2 );
	length += str.length;
	string[ length  ] = u'\0';
}

String &String::operator+=( const String &str ) {
	Append( str );
	return *this;
}

String String::operator+( const String &str ) const {
	String result = *this;
	result.Append( str );
	return result;
}

void String::Clear() {
	string[ 0 ] = u'\0';
	length = 0;
}

void String::Insert( const String &str, const int index ) {
	if ( str.length <= 0 ) {
		return;
	}
	if ( index < 0 || index > length ) {
		return;
	}
	int newLength = length + str.length;
	
	// pokud neni potreba realokovat pamet:
	if ( newLength < capacity ) {
		memmove( string + index + str.length, string + index, str.length * 2 );
		memcpy( string + index, str.string, str.length * 2 );
		length = newLength;
		string[ length ] = u'\0';
		return;
	}
	// alokovat novou pamet
	char16_t *result = new char16_t[ newLength + 1 ];
	memcpy( result, string, index * 2 );
	memcpy( result + index, str.string, str.length * 2 );
	memcpy( result + index + str.length, string + index, ( length - index ) * 2 );
	delete [] stringLong;
	stringLong = result;
	string = stringLong;
	capacity = newLength + 1;
	length = newLength;
	string[ length ] = u'\0';
}

void String::Join( const String * const strings[], const int count, String separator, String &result ) {
	if ( strings == nullptr || count <= 0 ) {
		result.Clear();
		return;
	}
	// vypocet delky vysledneho retezce
	int size = 0;
	for ( int i = 0; i < count; i++ ) {
		size += strings[ i ]->length;
	}
	if ( count > 1 ) {
		size += ( count - 1 ) * separator.length;
	}
	result.Alloc( size + 1 );
	
	// spojeni
	for ( int i = 0; i < count - 1; i++ ) {
		result.Append( *strings[ i ] );
		result.Append( separator );
	}
	result.Append( *strings[ count - 1 ] );
}

void String::Join( const String strings[], const int count, String separator, String &result ) {
	if ( strings == nullptr || count <= 0 ) {
		result.Clear();
		return;
	}
	// vypocet delky vysledneho retezce
	int size = 0;
	for ( int i = 0; i < count; i++ ) {
		size += strings[ i ].length;
	}
	if ( count > 1 ) {
		size += ( count - 1 ) * separator.length;
	}
	result.Alloc( size + 1 );
	
	// spojeni
	for ( int i = 0; i < count - 1; i++ ) {
		result.Append( strings[ i ] );
		result.Append( separator );
	}
	result.Append( strings[ count - 1 ] );
}

int String::Replace( const char16_t symbol, const char16_t replace ) {
	int count = 0;
	for ( int i = 0; i < length; i++ ) {
		if ( string[ i ] == symbol ) {
			string[ i ] = replace;
			count += 1;
		}
	}
	return count;
}

int String::Find( const char16_t ch, const int start ) const {
	for ( int i = start; i < length; i++ ) {
		if ( string[ i ] == ch ) {
			return i;
		}
	}
	return -1;
}

int String::FindBack( const char16_t ch, const int start ) const {
	int beg = ( start == MAX_LENGTH ? length - 1 : start );
	for ( int i = beg; i >= 0; i-- ) {
		if ( string[ i ] == ch ) {
			return i;
		}
	}
	return -1;
}

uint32_t String::Hash() const {
	uint32_t hash = 101;
	for ( int i = 0; i < length; i++ ) {
		hash += 101 * static_cast< uint32_t >( string[ i ] );
	}
	return hash;
}