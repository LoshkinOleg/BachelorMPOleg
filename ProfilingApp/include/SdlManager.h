#pragma once

#include <functional>

#include <SDL.h>
#undef main

#include "BSExpCommon.h"

namespace bsExp
{
	class SdlManager
	{
	public:
		BSEXP_NON_COPYABLE(SdlManager);
		BSEXP_NON_MOVEABLE(SdlManager);

		SdlManager() = delete;
		SdlManager(const std::function<void(void)>& leftTriggerCallback,
				   const std::function<void(void)>& rightTriggerCallback,
				   const std::function<void(void)>& leftGripCallback,
				   const std::function<void(void)>& leftPadCallback,
				   const std::function<void(void)>& escapeCallback
		);
		~SdlManager();

		bool Update();

	private:
		SDL_Event sdlEvent_{};
		SDL_Window* sdlWindow_ = nullptr;
		bool shutdown_ = false;

		const std::function<void(void)>& leftTriggerCallback_, rightTriggerCallback_, leftGripCallback_, leftPadCallback_, escapeCallback_;
	};
}