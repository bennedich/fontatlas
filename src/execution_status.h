#pragma once

#include "types.h"

namespace fontatlas
{
	enum class ReturnStatus : u8
	{
		OK,
		FAIL_INIT_FREETYPE,
		FAIL_LOAD_FONT,
		FAIL_OPEN_METADATA_FILE,
		FAIL_OPEN_DDS_FILE,
	};
}