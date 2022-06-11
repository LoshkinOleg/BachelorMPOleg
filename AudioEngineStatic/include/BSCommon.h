#pragma once

namespace bs
{
	enum class ClipWrapMode
	{
		ONE_SHOT,
		LOOP
	};

	using SoundMakerId = size_t;
	constexpr const size_t INVALID_ID = (size_t)-1;
}