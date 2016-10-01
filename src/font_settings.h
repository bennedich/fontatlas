#pragma once

#include "types.h"
#include "font_format.h"

namespace fontatlas
{
	struct FontSettings
	{
		std::string              font_name;
		std::string              font_path;
		FontFormat               format;
		u32                      atlas_w;
		u32                      atlas_h;
		u32                      glyph_w;
		u32                      glyph_h;
		u32                      channels_n;
		double                   angle_threshold;
		double                   range;
		double                   scale;
		std::pair<double,double> translation;
	};
}