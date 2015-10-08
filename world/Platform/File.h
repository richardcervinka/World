#pragma once

#include "Platform.h"
#include "..\Framework\Types.h"
#include "..\Framework\String.h"
#include "..\Framework\Array.h"

// Vrati cely nazev souboru, bez adresarove cesty
String GetFileName( const String &file );

// Vriti cisty nazev souboru bez pripony: "data/images/image.png" -> "image"
String GetFileBase( const String &file );

// Vrati priponu souboru ("png")
String GetFileExt( const String &file );

// vrati cestu k souboru: "data/images/image.png" -> "data/images/"
String GetFileDir( const String &file );

enum class FileMode {
	READ,
	WRITE
};

enum class FileAccess {
	DEFAULT,
	SEQUENTIAL,
	RANDOM
};

// Rozhrani tridy File. Cesta je vzdy relativni, oddelovac je znak '/'
//
class IFile {
public:
	enum {
		EOF = ULONG_MAX
	};
	
	IFile();
	virtual ~IFile();
	
	// neni mozne vytvaret kopie objektu
	IFile( const IFile& ) = delete;
	IFile &operator=( const IFile& ) = delete;
	
	// Otevre existujici soubor v rezimu READ
	virtual bool OpenToRead( const String &fullname, const FileAccess access ) = 0;
	
	// Otevre existujici soubor v rezimu WRITE
	virtual bool OpenToWrite( const String &fullname, const FileAccess access ) = 0;
	
	// Vytvori novy soubor v rezimu WRITE, pokud soubor uz existuje, vrati false
	virtual bool Create( const String &fullname ) = 0;
	
	// Vytvori novy soubor v rezimu WRITE, pokud soubor uz existuje, bude zkracen na nulovou velikost.
	virtual bool CreateNew( const String &fullname ) = 0;
	
	// Zavre otevreny soubor
	virtual void Close() = 0;
	
	virtual bool IsOpen() const = 0;
	
	// Zkrati soubor na nulovou velikost
	virtual void Clear() = 0;
	
	// Nacte pozadovany pocet bajtu ze souboru do bufferu, vraci pocet nactenych bajtu
	virtual unsigned long Read( void * const buffer, const unsigned long bytes ) = 0;
	
	// Zapise do souboru obsah bufferu, vraci pocet zapsanych bajtu
	virtual unsigned long Write( const void * const buffer, const unsigned long bytes ) = 0;
	
	// vrati velikost souboru v bajtech
	virtual unsigned long Size() const = 0;
	
	// Vrati aktualni pozici ukazatele (ekvivalent tellg())
	virtual unsigned long GetPointer() const = 0;
	
	// nastavi ukazatel na pozici od zacatku souboru, muze mit hodnotu File::EOF, vraci vyslednou pozici
	virtual unsigned long SetPointer( const unsigned long position ) = 0;
	
	// posune ukazatel o hodnotu distance, vraci novou pozici
	virtual unsigned long MovePointer( const int distance ) = 0;
	
	// Vsechna data v bufferech jsou ihned zapsana do souboru
	virtual void Flush() = 0;
	
	// Parsovani nazvu souboru
	virtual const String GetName() const = 0;
	virtual const String GetExt() const = 0;
	virtual const String GetBase() const = 0;
	virtual const String GetDir() const = 0;
	
	// Nacte zakladni datove typy ulozene v souboru jako little-endian
	virtual Byte ReadByte();
	virtual Int32 ReadInt32();
	virtual Uint32 ReadUint32();
	virtual float ReadFloat();
	virtual Float2 ReadFloat2();
	virtual Float3 ReadFloat3();
	virtual Float4 ReadFloat4();
	
	// Ulozi zakladni datove typy jako little-endian
	// Data nejsou zarovnana (aligned), tzn. ze typ Float3 se ulozi jako posloupnost 3 x float
	virtual void WriteByte( const Byte value );
	virtual void WriteInt32( const Int32 value );
	virtual void WriteUint32( const Uint32 value );
	virtual void WriteFloat( const float value );
	virtual void WriteFloat2( const Float2 &value );
	virtual void WriteFloat3( const Float3 &value );
	virtual void WriteFloat4( const Float4 &value );
};

// Operace se soubory a adresari
//
namespace FileSystem {
	
	// Vytvori novy adresar
	bool CreateDir( const String &path );
	
	// odstrani existujici prazdny adresar
	bool RemoveDir( const String &path );
	
	// odstrani veskery obsah adresare
	bool RemoveDirContent( const String &path );
	
	// odstrani soubor
	bool RemoveFile( const String &fullname );
	
	// RemoveFiles
	// RemoveDirs
	
	// vrati seznam vsech souboru v adresari, napr. "dir/"
	// je mozne uvest i filtr pro nazev souboru, napr. "dir/*.png"
	bool EnumFiles( const String &path, Array< String > &result );
	
	// vrati seznam vsech primych podadresaru
	bool EnumDirs( const String &path, Array< String > &result );
};

// Implementace

#ifdef PLATFORM_WINDOWS
#include "windows/WindowsFile.h"
#define File FileWindows
#define FileSystemPlatform FileSystemWindows
#endif