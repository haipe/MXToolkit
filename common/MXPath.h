#pragma once

#include "MXCommon.h"

namespace mxtoolkit
{
	class Path
	{
	public:

		static bool	GetFilePathInfo( const std::string& file, std::string* path, std::string* name, std::string* ext );
		static bool	GetFilePathInfo( const std::wstring& file, std::wstring* path, std::wstring* name, std::wstring* ext );

		static bool	GetFilePathInfo( const std::string& file, std::string* path, std::string* name );
		static bool GetFilePathInfo( const std::wstring& file, std::wstring* path, std::wstring* name );

        static bool	GetFolderPathInfo(const std::string& folder, std::string* path, std::string* name);
        static bool GetFolderPathInfo(const std::wstring& folder, std::wstring* path, std::wstring* name);

		static std::string	GetFileExtName( const std::string& file );
		static std::wstring	GetFileExtName( const std::wstring& file );

#ifndef WIN32
		//创建目录，多级创建
		static int CreateDir( const CHAR* path );

#endif
#ifdef WIN32
		static bool GetMD5( const std::wstring& file, std::wstring* md5 );

#endif
	};

}