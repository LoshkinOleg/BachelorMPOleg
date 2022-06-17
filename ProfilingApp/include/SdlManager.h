#pragma once

#include <functional>
#include <map>

#include <SDL.h>
#undef main

#include "BSCommon.h"

namespace bsExp
{
	class SdlManager
	{
	public:
		enum class Input: size_t
		{
			LeftTrigger,
			RightTrigger,
			LeftGrip,
			RightGrip,
			LeftPad,
			RightPad,
			Spacebar,
			Enter,
			Backspace
		};

		BS_NON_COPYABLE(SdlManager);
		BS_NON_MOVEABLE(SdlManager);

		SdlManager();
		~SdlManager();

		void RegisterCallback(const Input input, const std::function<void(void)>& callback);
		bool Update();

	private:
		SDL_Event sdlEvent_{};
		SDL_Window* sdlWindow_ = nullptr;

		std::map<Input, std::vector<std::function<void(void)>>> callbacks_;
	};
}