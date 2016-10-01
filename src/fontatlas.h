#pragma once

#include <algorithm>
#include <fstream>
#include <memory>
#include <regex>
#include <stdio.h>

#include <msdfgen.h>
#include <msdfgen-ext.h>

#include FT_FREETYPE_H
#include <ft2build.h>
#include <freetype/ftglyph.h>
#include <lodepng.h>

#include "types.h"
#include "log.h"
#include "dds_header.h"
#include "font_format.h"
#include "font_settings.h"
#include "execution_status.h"

namespace
{
	using namespace fontatlas;
	using fontatlas::log;

	void _write_glyph_metadata( const FontSettings& settings, const msdfgen::FontHandle* font, MetaData& metadata )
	{
		log( "*** Generate font metadata." );

		FT_Vector delta;
		FT_GlyphSlot g = font->face->glyph;

		u32 chars_n = settings.atlas_w * settings.atlas_h;
		u32 i = 0u;

		float offset_scale = 128.f / 64.f / float(settings.glyph_w);

//		FT_Load_Glyph( font->face, 'W', FT_LOAD_RENDER );
//		FT_Get_Kerning( font->face, 'W', 'S', FT_KERNING_DEFAULT, &delta );
//		log( "********************** %d", i8(offset_scale * float(g->advance.x + delta.x)) );

		for ( u32 c1 = 0u; c1 < chars_n; ++c1 )
		{
			u32 i1 = FT_Get_Char_Index( font->face, c1 );
			FT_Load_Glyph( font->face, i1, FT_LOAD_RENDER );

			for ( u32 c2 = 0u; c2 < chars_n; ++c2 )
			{
				u32 i2 = FT_Get_Char_Index( font->face, c2 );
				FT_Get_Kerning( font->face, i1, i2, FT_KERNING_DEFAULT, &delta );
				i8 offset = i8(offset_scale * float(g->advance.x + delta.x));
				metadata[ i++ ] = offset;
			}
		}
	}

	void _read_glyph_msdf( const FontSettings& settings, msdfgen::FontHandle* font, const u16 c, msdfgen::Shape& shape, msdfgen::Bitmap< msdfgen::FloatRGB >& bitmap )
	{
		msdfgen::edgeColoringSimple( shape, settings.angle_threshold );
		msdfgen::generateMSDF( bitmap, shape, settings.range, settings.scale, msdfgen::Vector2(settings.translation.first, settings.translation.second) );
	}

	void _read_glyph_freetype( const FontSettings& settings, msdfgen::FontHandle* font, const u16 c, msdfgen::Shape& shape, msdfgen::Bitmap< msdfgen::FloatRGB >& bitmap )
	{
		FT_Load_Char( font->face, c, FT_LOAD_RENDER );
		FT_GlyphSlot g = font->face->glyph;

		//log( "*** %d x %d", g->bitmap_left, g->bitmap_top );
		//log( "*** %d x %d", g->advance.x, g->advance.y );

//		FT_Glyph glyph;
//		FT_BitmapGlyph glyph_bitmap;
//
//		FT_Load_Char( font->face, c, FT_LOAD_RENDER );
//		FT_Get_Glyph( g, &glyph );
//		FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, NULL, true );
//		glyph_bitmap = (FT_BitmapGlyph)glyph;

		// BEWARE !! dds is BGR because microsoft!

		for ( u32 i = 0u, n = bitmap.width(); i < n; ++i )
		{
			for ( u32 j = 0u, m = bitmap.height(); j < m; ++j )
			{
				if ( i == 0 || i == n-1 || j == 0 || j == m-1 )
					bitmap(i,j) = {0,0,1};
				else
					bitmap(i,j) = {1,0,0};
			}
		}

		u32 i = 0u;
		i32 off_x = settings.translation.first;
		i32 off_y = settings.translation.second;

		for ( i32 y = std::min((u32)bitmap.height(), (u32)(g->bitmap.rows + off_y)) - 1u; y >= off_y; --y )
		{
			for ( i32 x = off_x, w = std::min((u32)bitmap.width(), (u32)(g->bitmap.width + off_x)); x < w; ++x )
			{
				i32 final_y = y + g->bitmap_top - g->bitmap.rows;
				if ( 0u <= final_y && final_y < bitmap.height() )
				{
					msdfgen::FloatRGB & rgb = bitmap( x, final_y );

//				rgb.r = glyph_bitmap->bitmap.buffer[ i ];
//				rgb.g = glyph_bitmap->bitmap.buffer[ i ];
//				rgb.b = glyph_bitmap->bitmap.buffer[ i ];

					rgb.r = g->bitmap.buffer[ i ];
					rgb.g = g->bitmap.buffer[ i ];
					rgb.b = g->bitmap.buffer[ i ];
				}

				++i;
			}
		}

//		FT_Done_Glyph( glyph );
	}

	void _write_glyph_atlas( const FontSettings& settings, msdfgen::Bitmap< msdfgen::FloatRGB >& bitmap, std::vector<unsigned char>& atlas, const u16 c )
	{
		u32 char_x = c % settings.atlas_w;
		u32 char_y = c / settings.atlas_w;

		u32 atlas_dy = settings.channels_n * settings.glyph_w * settings.atlas_w;
		u32 atlas_y0 = atlas_dy * settings.glyph_h * char_y;
		u32 atlas_y1 = atlas_dy * settings.glyph_h + atlas_y0;

		u32 atlas_x0 = settings.channels_n * settings.glyph_w * char_x;
		u32 atlas_x1 = settings.channels_n * settings.glyph_w + atlas_x0;

		u32 glyph_y = settings.glyph_h;
		u32 atlas_y = atlas_y0;

		while ( atlas_y < atlas_y1 )
		{
			--glyph_y;

			u32 glyph_x = 0u;
			u32 atlas_x = atlas_x0;

			while ( atlas_x < atlas_x1 )
			{
				msdfgen::FloatRGB & rgb = bitmap( glyph_x, glyph_y );
				++glyph_x;

				atlas[ atlas_y + atlas_x++ ] = std::max( 0, std::min(255, int(255 * rgb.r)) );
				atlas[ atlas_y + atlas_x++ ] = std::max( 0, std::min(255, int(255 * rgb.g)) );
				atlas[ atlas_y + atlas_x++ ] = std::max( 0, std::min(255, int(255 * rgb.b)) );
			}

			atlas_y += atlas_dy;
		}
	}

	void _write_glyph_atlas( const FontSettings& settings, msdfgen::FontHandle* font, std::vector<unsigned char>& atlas )
	{
		log( "*** Generate font atlas '%s'.", settings.format == FontFormat::MSDF ? "MSDF" : "PIXEL" );

		u16 chars_n = settings.atlas_w * settings.atlas_h;

		for ( u16 c = 0u; c < chars_n; ++c )
		{
			msdfgen::Shape shape;
			if ( !msdfgen::loadGlyph(shape, font, c) )
			{
				log( "Couldnt find char '", c, "'" );
				continue;
			}

			shape.normalize();
			msdfgen::Bitmap< msdfgen::FloatRGB > bitmap( settings.glyph_w, settings.glyph_h );

			FT_Load_Char( font->face, c, FT_LOAD_RENDER );
			FT_GlyphSlot g = font->face->glyph;

			log( "%c :: %d x %d ::      %d x %d",
				 c,
				 c % settings.atlas_w,
				 c / settings.atlas_w,
				 g->bitmap.width,
				 g->bitmap.rows );

			if ( settings.format == FontFormat::MSDF )
				_read_glyph_msdf( settings, font, c, shape, bitmap );
			else
				_read_glyph_freetype( settings, font, c, shape, bitmap );

			_write_glyph_atlas( settings, bitmap, atlas, c );
		}
	}

	ReturnStatus _generate_font( const FontSettings& settings, msdfgen::FreetypeHandle* ft )
	{
		msdfgen::FontHandle* font = msdfgen::loadFont( ft, settings.font_path.c_str() );
		if ( !font )
			return ReturnStatus::FAIL_LOAD_FONT;

		//FT_Set_Char_Size( font->face, 2000, 2000, 0, 0 );
		FT_Set_Pixel_Sizes( font->face, settings.glyph_w>>0, settings.glyph_h>>0 ); // This is required if we want to read glyph bitmap.

		u16 chars_n = settings.atlas_w * settings.atlas_h;
		u16 glyph_size = settings.glyph_w * settings.glyph_h;
		auto metadata = MetaData( chars_n * chars_n );
		auto atlas = std::vector<unsigned char>( settings.channels_n * glyph_size * chars_n );

		_write_glyph_metadata( settings, font, metadata );
		_write_glyph_atlas( settings, font, atlas );

		msdfgen::destroyFont( font );

		std::string filename;
		{
			std::regex filename_regex( "^(?:.*/|.*\\\\)*([^.]+)(?:\\.\\w+)*$" );
			std::smatch filename_match;

			if ( std::regex_match(settings.font_path, filename_match, filename_regex) )
			{
				filename = filename_match[ filename_match.size()-1 ];
			}
			else
			{
				filename = "OUT";
			}
		}

		{
			std::string filename_metadata = filename + ".meta";
			FILE *f = fopen( filename_metadata.c_str(), "wb" );
			if ( !f )
			{
				return ReturnStatus::FAIL_OPEN_METADATA_FILE;
			}
			fwrite( metadata.data(), sizeof(metadata[0]), metadata.size(), f );
			fclose( f );
		}

		{
			u32 width = settings.glyph_w * settings.atlas_w;
			u32 rgb_bit_count = settings.channels_n * 8;

			DdsHeader header;
			header.dw_flags |= DDSD_PITCH;
			header.dw_height = settings.glyph_h * settings.atlas_h;
			header.dw_width = width;
			header.dw_pitch_or_linear_size = (width * rgb_bit_count + 7u) / 8u;
			header.dw_depth = 0;
			header.dw_mipmap_levels = 0;
			header.pixel_format.dw_flags |= DDPF_RGB;
			header.pixel_format.dw_rgb_bit_count = rgb_bit_count;
			header.pixel_format.dw_r_bitmask = 0x00ff0000;
			header.pixel_format.dw_g_bitmask = 0x0000ff00;
			header.pixel_format.dw_b_bitmask = 0x000000ff;
			//header.pixel_format.dw_a_bitmask = 0xff000000;
			header.caps.dw_caps1 = DDSCAPS_TEXTURE;

			std::string filename_atlas = filename + ".dds";
			FILE *f = fopen( filename_atlas.c_str(), "wb" );
			if ( !f )
			{
				return ReturnStatus::FAIL_OPEN_DDS_FILE;
			}
			fwrite( &header, sizeof(header), 1, f );
			fwrite( atlas.data(), sizeof(atlas[0]), atlas.size(), f );
			fclose( f );
		}

		{
			lodepng::encode( filename + ".png", atlas.data(), settings.atlas_w*settings.glyph_w, settings.atlas_h*settings.glyph_h, LCT_RGB );
		}

		return ReturnStatus::OK;
	}
}

namespace fontatlas
{
	ReturnStatus generate( const FontSettings& settings )
	{
		msdfgen::FreetypeHandle* ft = msdfgen::initializeFreetype();
		if ( !ft )
			return ReturnStatus::FAIL_INIT_FREETYPE;

		auto status = _generate_font( settings, ft );

		msdfgen::deinitializeFreetype( ft );

		return status;
	}
}
