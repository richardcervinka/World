#pragma once

#include "Types.h"

// Pouziva UCS-2 kodovani; reprezentuje BMP mnozinu UTF-16,
// poku je potreba prevest String na char16_t, neni nutne provadet zadnou konverzi.
// Vzdy pouzivat literar u (string = u"hello");
// Pretypovani z UTF-8 je mozne jen prostrednictvim funkce FromUTF8()
//
class String {
public:
	enum {
		SHORT_LENGTH = 64,
		MAX_LENGTH = INT_MAX
	};
	
	String();
	String( const String &str );
	explicit String( const char16_t * const strUTF16 );
	~String();
	
	// vytvori substring, pokud jsou parametry mimo rozsah, vytvori prazdny retezec
	String( const String &str, const int index, const int size );
	
	// prirazeni
	String &operator=( const String &str );
	String &operator=( const char16_t * const strUTF16 );
	
	// Inicializuje retezec rozsahem retezce str, pri neplatnych parametrech zkrati retezec na nulovou delku
	void Set( const String &str, const int index, const int size );
	
	// vrati ukazatel na char16_t* string
	const char16_t *Raw() const;
	
	// nezadavat parametr str pomoci uvozovek, kde neni definovane, jake kodovani je pouzito!
	void FromUCS( const char * const str );
	
	// nezadavat parametr str pomoci uvozovek, kde neni definovane, jake kodovani je pouzito!
	void FromUCS2( const char16_t * const str );
	
	// Provede poze pretypovani, tedy znaky budou v rozsahu 0-255 
	void FromUTF8( const char * const str );
	
	// znaky vetsi nez 0x010000 jsou nahrazeny hodnotou 0xffff
	void FromUTF16( const char16_t * const str );
	
	// Rozsiri alokovanou pamet na hodnotu vetsi nez je aktualne alokovana pamet.
	// Zkrati ulozeny retezec na nulovou delku, neprovadi se tedy kopirovani puvodniho retezce do nove pameti.
	void Alloc( const int count );
	
	// Zmeni velikost alokovane pameti minimalne pro SHORT_LENGTH znaku.
	// Zkrati ulozeny retezec na nulovou delku
	void Realloc( const int count );
	
	// Rozsiri alokovanou pamet pro pozadovany pocet znaku.
	// Pozadovana pamet musi byt vetsi nez aktualne alokovana pamet, jinak nedela nic.
	void Expand( const int size );
	
	// delka ulozeneho retezce
	int Length() const;
	
	// prevod mizi uppercase a lowercase
	void ToUpper();
	void ToLower();
	
	// pristup k jednotlivym znakum kodovnym v UCS-2, z duvodu efektivity se neprovadi kontrola rozsahu!
	char16_t &operator[]( const int index );
	const char16_t &operator[]( const int index ) const;
	
	// operatory porovnani
	bool operator==( const String &str ) const;
	bool operator!=( const String &str ) const;
	bool operator>( const String &str ) const;
	bool operator>=( const String &str ) const;
	bool operator<( const String &str ) const;
	bool operator<=( const String &str ) const;
	
	// spojovani retezcu
	//void Append( const String strings[], const int count );
	void Append( const String &str );
	String &operator+=( const String &str );
	String operator+( const String &str ) const;
	
	// Vlozi retezec na urcenou pozici; pri vlozeni "foo" do "bar" na pozici 1 vznikne retezec "bfooar"
	// Pokud je index mimo rozsah nedela nic.
	void Insert( const String &str, const int index );
	
	// Nastavi prazdny retezec: string = u"";
	void Clear();
	
	// porovnani, vraci stejny vysledek jako memcmp()
	int Compare( const String &str ) const;
	
	// nahradi vsechny znaky "symbol" znakem "replace", vraci pocet nahrazenych znaku
	int Replace( const char16_t symbol, const char16_t replace );
	
	// Vrati index prvniho vyskytu znaku ch, jinak vraci zapornou hodnotu. Vyhledavani zacina od indexu "start"
	int Find( const char16_t ch, const int start = 0 ) const;
	
	// Stejne jako Find(), prohledava se ale v opacnem smeru (odzadu)
	int FindBack( const char16_t ch, const int start = MAX_LENGTH ) const;
	
	// vypocita hash
	Uint32 Hash() const;
	
	// vrati velikost retezce v prislusnem kodovani
	static int LengthUCS2( const char16_t * const str );
	static int LengthUTF8( const char * const str );
	static int LengthUTF16( const char16_t * const str );
	
	// spojeni retezcu
	static void Join( const String * const strings[], const int count, String separator, String &result );
	static void Join( const String strings[], const int count, String separator, String &result );
	
private:
	int length;
	int capacity;
	char16_t *string;
	char16_t *stringLong;
	char16_t stringShort[ SHORT_LENGTH ];
};

inline const char16_t *String::Raw() const {
	return string;
}

inline char16_t &String::operator[]( const int index ) {
	return string[ index ];
}

inline const char16_t &String::operator[]( const int index ) const {
	return string[ index ];
}

inline int String::Length() const {
	return length;
}