#undef _UNICODE
#undef UNICODE

#include <Windows.h>
#include <cwchar>
#include "..\File.h"
#include "WindowsFile.h"

// class FileWindows

FileWindows::FileWindows() {
	handle = 0;
}

FileWindows::~FileWindows() {
	FileWindows::Close();
}

bool FileWindows::CreateFile( const String& fullname, const FileMode mode, const FileAccess access, const unsigned int disposition ) {
	if ( handle != 0 ) {
		return false;
	}
	DWORD fileAccess = 0;
	DWORD fileShare = 0;
	switch ( mode ) {
	case FileMode::READ:
		fileAccess = GENERIC_READ;
		fileShare = FILE_SHARE_READ;
		break;

	case FileMode::WRITE:
		fileAccess = GENERIC_WRITE;
		fileShare = 0;
		break;
	}
	
	DWORD fileFlags = FILE_ATTRIBUTE_NORMAL;
	switch ( access ) {
	case FileAccess::SEQUENTIAL:
		fileFlags |= FILE_FLAG_SEQUENTIAL_SCAN;
		break;
		
	case FileAccess::RANDOM:
		fileFlags |= FILE_FLAG_RANDOM_ACCESS;
		break;
		
	case FileAccess::DEFAULT:
		break;
	}
	
	HANDLE hfile = ::CreateFileW(
		reinterpret_cast< LPCWSTR >( fullname.Raw() ),
		fileAccess,
		fileShare,
		NULL,
		static_cast< DWORD >( disposition ),
		fileFlags,
		NULL
	);
	if ( hfile == INVALID_HANDLE_VALUE ) {
		return false;
	}
	// ulozit vysledek
	handle = reinterpret_cast< unsigned long >( hfile );
	this->fullname = fullname;
	this->mode = mode;
	return true;
}

bool FileWindows::OpenToRead( const String& fullname, const FileAccess access ) {
	return CreateFile( fullname, FileMode::READ, access, OPEN_EXISTING );
}

bool FileWindows::OpenToWrite( const String& fullname, const FileAccess access ) {
	return CreateFile( fullname, FileMode::WRITE, access, OPEN_EXISTING );
}

bool FileWindows::Create( const String& fullname ) {
	return CreateFile( fullname, FileMode::WRITE, FileAccess::SEQUENTIAL, CREATE_NEW );
}

bool FileWindows::CreateNew( const String& fullname ) {
	return CreateFile( fullname, FileMode::WRITE, FileAccess::SEQUENTIAL, CREATE_ALWAYS );
}

void FileWindows::Close() {
	if ( handle != 0 ) {
		CloseHandle( reinterpret_cast< HANDLE >( handle ) );
		handle = 0;
		fullname.Clear();
	}
}

bool FileWindows::IsOpen() const {
	return handle != 0;
}

const String FileWindows::GetName() const {
	return GetFileName( fullname );
}

const String FileWindows::GetExt() const {
	return GetFileExt( fullname );
}

const String FileWindows::GetBase() const {
	return GetFileBase( fullname );
}

const String FileWindows::GetDir() const {
	return GetFileDir( fullname );
}

unsigned long FileWindows::Size() const {
	if ( handle == 0 ) {
		return 0;
	}
	LARGE_INTEGER size;
	GetFileSizeEx( reinterpret_cast< HANDLE >( handle ), &size );
	return static_cast< unsigned long >( size.QuadPart );
}

unsigned long FileWindows::Read( void* const buffer, const unsigned long bytes ) {
	if ( handle == 0 ) {
		return 0;
	}
	if ( mode != FileMode::READ ) {
		return 0;
	}
	DWORD reads = 0;
	ReadFile( reinterpret_cast< HANDLE >( handle ), buffer, static_cast< DWORD >( bytes ), &reads, NULL );
	return static_cast< unsigned long >( reads );
}

unsigned long FileWindows::Write( const void* const buffer, const unsigned long bytes ) {
	if ( handle == 0 ) {
		return 0;
	}
	if ( mode != FileMode::WRITE ) {
		return 0;
	}
	DWORD writes = 0;
	WriteFile( reinterpret_cast< HANDLE >( handle ), buffer, static_cast< DWORD >( bytes ), &writes, NULL );
	return static_cast< unsigned long >( writes );
}

void FileWindows::Clear() {
	if ( handle == 0 ) {
		return;
	}
	SetFilePointer( reinterpret_cast< HANDLE >( handle ), 0, NULL, FILE_BEGIN );
	SetEndOfFile( reinterpret_cast< HANDLE >( handle ) );
}

unsigned long FileWindows::GetPointer() const {
	if ( handle == 0 ) {
		return 0;
	}
	LARGE_INTEGER offset;
	offset.QuadPart = 0;
	LARGE_INTEGER pointer;
	SetFilePointerEx( reinterpret_cast< HANDLE >( handle ), offset, &pointer, FILE_CURRENT );
	return static_cast< unsigned long >( pointer.QuadPart );
}

unsigned long FileWindows::SetPointer( const unsigned long position ) {
	if ( handle == 0 ) {
		return 0;
	}
	LARGE_INTEGER pointer;
	if ( position == IFile::END_OF_FILE ) {
		LARGE_INTEGER offset;
		offset.QuadPart = 0;
		SetFilePointerEx( reinterpret_cast< HANDLE >( handle ), offset, &pointer, FILE_END );
	} else {
		LARGE_INTEGER offset;
		offset.QuadPart = static_cast< LONGLONG >( position );
		SetFilePointerEx( reinterpret_cast< HANDLE >( handle ), offset, &pointer, FILE_BEGIN );
	}
	return static_cast< unsigned long >( pointer.QuadPart );
}

unsigned long FileWindows::MovePointer( const int distance ) {
	if ( handle == 0 ) {
		return 0;
	}
	LARGE_INTEGER offset;
	offset.QuadPart = static_cast< LONGLONG >( distance );
	LARGE_INTEGER pointer;
	SetFilePointerEx( reinterpret_cast< HANDLE >( handle ), offset, &pointer, FILE_CURRENT );
	return static_cast< unsigned long >( pointer.QuadPart );
}

void FileWindows::Flush() {
	if ( handle != 0 ) {
		FlushFileBuffers( reinterpret_cast< HANDLE >( handle ) );
	}
}

// FileSystemWindows

bool FileSystemWindows::CreateDir( const String& path ) {
	return CreateDirectoryW( reinterpret_cast< LPCWSTR >( path.Raw() ), NULL ) == TRUE;
}

bool FileSystemWindows::RemoveDir( const String& path ) {
	return RemoveDirectoryW( reinterpret_cast< LPCWSTR >( path.Raw() ) ) == TRUE;
}

bool FileSystemWindows::RemoveDirContent( const String& path ) {
	// not implemented
	return false;
}

bool FileSystemWindows::RemoveFile( const String& fullname ) {
	return DeleteFileW( reinterpret_cast< LPCWSTR >( fullname.Raw() ) ) == TRUE;
}

enum class FindFilesMode {
	FILE,
	DIR,
	ALL
};

bool FindFiles( const String& path, FindFilesMode mode, std::vector< String >& result ) {
	String fullname = path;
	if ( fullname[ fullname.Length() - 1 ] == u'/' ) {
		fullname += String( u"*.*" );
	}
	
	WIN32_FIND_DATAW fd;
	HANDLE handle = FindFirstFileW( reinterpret_cast< LPCWSTR >( fullname.Raw() ), &fd );
	if ( handle == INVALID_HANDLE_VALUE ) {
		return false;
	}
	
	// vymazat obsah pole az pokud je platny handle a funkce vrati true
	result.clear();
	
	// prochazet vsechny objekty v adresari
	do {
		bool isDir = ( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;
		
		// v rezimu FindFilesMode::FILE preskocit adresare
		if ( mode == FindFilesMode::FILE && isDir ) {
			continue;
		}
		// v rezimu FindFilesMode::DIR preskocit soubory
		if ( mode == FindFilesMode::DIR && !isDir ) {
			continue;
		}
		// preskocit adresare s nazvem "." a ".."
		if ( wcsncmp( fd.cFileName, L".", 1 ) == 0 ) {
			continue;
		}
		if ( wcsncmp( fd.cFileName, L"..", 2 ) == 0 ) {
			continue;
		}
		result.push_back( String( reinterpret_cast< char16_t* >( fd.cFileName ) ) );
		
	} while ( FindNextFileW( handle, &fd ) );
	
	FindClose( handle );
	return true;
}

bool FileSystemWindows::EnumFiles( const String& path, std::vector< String >& result ) {
	return FindFiles( path, FindFilesMode::FILE, result );
}

bool FileSystemWindows::EnumDirs( const String& path, std::vector< String >& result ) {
	return FindFiles( path, FindFilesMode::DIR, result );
}