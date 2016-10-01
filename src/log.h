#pragma once

#include <string>
#include <stdio.h>

namespace fontatlas
{
	void log_void( const std::string& format, ... ) {}

	void log_default( const std::string& format, ... )
	{
		const std::string final_format = format + "\n";
		va_list argptr;
		va_start(argptr, final_format.c_str() );
		vfprintf(stderr, final_format.c_str(), argptr);
		va_end(argptr);
	}

	using Logger = void(*)( const std::string& format, ... );

	Logger log = log_void;
}
