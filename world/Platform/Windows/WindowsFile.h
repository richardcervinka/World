#pragma once

class FileWindows: public IFile {
public:
	using IFile::END_OF_FILE;

	FileWindows();
	virtual ~FileWindows();
	
	// implementace rozhrani IFile
	virtual bool OpenToRead( const String& fullname, const FileAccess access ) override;
	virtual bool OpenToWrite( const String& fullname, const FileAccess access ) override;
	virtual bool Create( const String& fullname ) override;
	virtual bool CreateNew( const String& fullname ) override;
	virtual void Close() override;
	virtual bool IsOpen() const override;
	virtual void Clear() override;
	virtual unsigned long Read( void* const buffer, const unsigned long bytes ) override;
	virtual unsigned long Write( const void* const buffer, const unsigned long bytes ) override;
	virtual unsigned long Size() const override;
	virtual unsigned long GetPointer() const override;
	virtual unsigned long SetPointer( const unsigned long position ) override;
	virtual unsigned long MovePointer( const int distance ) override;
	virtual void Flush() override;
	virtual const String GetName() const override;
	virtual const String GetExt() const override;
	virtual const String GetBase() const override;
	virtual const String GetDir() const override;
	
private:
	bool CreateFile( const String& fullname, const FileMode mode, const FileAccess access, const unsigned int disposition );
	
private:
	unsigned long handle;
	FileMode mode;
	String fullname;
};

namespace FileSystemWindows {

	bool CreateDir( const String& path );
	bool RemoveDir( const String& path );
	bool RemoveDirContent( const String& path );
	bool RemoveFile( const String& fullname );
	bool EnumFiles( const String& path, std::vector< String >& result );
	bool EnumDirs( const String& path, std::vector< String >& result );
}
