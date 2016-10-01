#include <string>
#include <iostream>

#include "fontatlas.h"

int main( int argc, const char* args[] )
{
	if ( argc < 2 || !std::strcmp(args[1], "$@") )
	{
		std::cout << "Required 1st arg 'font path'." << std::endl;
		std::cout << "Optional 2nd arg 'pixel format' can be either MSDF or PIXEL (default)." << std::endl;
		return 0;
	}

	fontatlas::log = &fontatlas::log_default;

	std::cout << "*** Starting in: " << args[ 0 ] << std::endl;
	std::cout << "*** Initializing..." << std::endl;

	fontatlas::FontSettings settings;
	settings.font_path = args[ 1 ];
	settings.format = ( argc > 2 && !std::strcmp(args[2], "MSDF") ) ?
		fontatlas::FontFormat::MSDF :
		fontatlas::FontFormat::PIXEL;
	settings.atlas_w = 16u;
	settings.atlas_h = 16u;
	settings.glyph_w = 64u;
	settings.glyph_h = 64u;
	settings.channels_n = 3u;
	settings.angle_threshold = 3.0;
	settings.range = 4.0;
	settings.scale = 1.5;
	settings.translation = { 8.0, 12.0 };

	auto err = fontatlas::generate( settings );
	if ( err != fontatlas::ReturnStatus::OK )
	{
		std::cout << "*** Err: " << std::to_string((fontatlas::u8)err) << std::endl;
	}

	std::cout << "*** k thx bye" << std::endl;

	return 0;
}
