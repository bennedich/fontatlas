#pragma once

// Read more about DDS_HEADER structure here:
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb943982(v=vs.85).aspx

#include "types.h"

// DdsHeader.dw_flags
#define DDSD_CAPS                   0x00000001
#define DDSD_HEIGHT                 0x00000002
#define DDSD_WIDTH                  0x00000004
#define DDSD_PITCH                  0x00000008
#define DDSD_PIXELFORMAT            0x00001000
#define DDSD_MIPMAPCOUNT            0x00020000
#define DDSD_LINEARSIZE             0x00080000
#define DDSD_DEPTH                  0x00800000

// DdsHeader.pixel_format.dw_flags
#define DDPF_ALPHAPIXELS            0x00000001
#define DDPF_FOURCC                 0x00000004
#define DDPF_INDEXED                0x00000020
#define DDPF_RGB                    0x00000040

// DdsHeader.caps.dw_caps1
#define DDSCAPS_COMPLEX             0x00000008
#define DDSCAPS_TEXTURE             0x00001000
#define DDSCAPS_MIPMAP              0x00400000

// DdsHeader.caps.dw_caps2
#define DDSCAPS2_CUBEMAP            0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSCAPS2_VOLUME             0x00200000

namespace fontatlas
{
	struct DdsHeader
	{
		const u32 dw_magic = 0x20534444u; // Required, actually not part of header but put here for ease of use. Beware! LITTLE ENDIAN!
		const u32 dw_size = 124u; // Required header struct size (excluding dw_magic).
		u32 dw_flags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS; // Minimum required.
		u32 dw_height = 0u;
		u32 dw_width = 0u;
		u32 dw_pitch_or_linear_size = 0u;
		u32 dw_depth = 0u;
		u32 dw_mipmap_levels = 0u;
		u32 dw_reserved1[ 11 ] = { 0u,0u,0u,0u,0u,0u,0u,0u,0u,0u,0u };

		// DDPIXELFORMAT
		struct
		{
			const u32 dw_size = 32u; // Required pixel format struct size.
			u32 dw_flags = 0u;
			u32 dw_four_color_channels = 0u;
			u32 dw_rgb_bit_count = 0u;
			u32 dw_r_bitmask = 0u;
			u32 dw_g_bitmask = 0u;
			u32 dw_b_bitmask = 0u;
			u32 dw_a_bitmask = 0u;
		} pixel_format;

		// DDCAPS
		struct
		{
			u32 dw_caps1 = DDSCAPS_TEXTURE; // Minimum required.
			u32 dw_caps2 = 0u;
			u32 dw_caps3 = 0u;
			u32 dw_caps4 = 0u;
		} caps;

		u32 dw_reserved2 = 0u;
	};
}
