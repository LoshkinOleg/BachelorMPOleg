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
			One = SDL_SCANCODE_1,
			Two = SDL_SCANCODE_2,
			Three = SDL_SCANCODE_3,
			Four = SDL_SCANCODE_4,
			Five = SDL_SCANCODE_5,
			Six = SDL_SCANCODE_6,
			Seven = SDL_SCANCODE_7,
			Eight = SDL_SCANCODE_8,
			Nine = SDL_SCANCODE_9,
			Zero = SDL_SCANCODE_0,

			Spacebar = SDL_SCANCODE_SPACE,
			Enter = SDL_SCANCODE_RETURN,
			Backspace = SDL_SCANCODE_BACKSPACE,

			NumPlus = SDL_SCANCODE_KP_PLUS,
			NumMinus = SDL_SCANCODE_KP_MINUS,
			NumMultiply = SDL_SCANCODE_KP_MULTIPLY,
			NumDivide = SDL_SCANCODE_KP_DIVIDE,
			NumOne = SDL_SCANCODE_KP_1,
			NumTwo = SDL_SCANCODE_KP_2,
			NumThree = SDL_SCANCODE_KP_3,
			NumFour = SDL_SCANCODE_KP_4,
			NumFive = SDL_SCANCODE_KP_5,
			NumSix = SDL_SCANCODE_KP_6,
			NumSeven = SDL_SCANCODE_KP_7,
			NumEight = SDL_SCANCODE_KP_8,
			NumNine = SDL_SCANCODE_KP_9,
			NumZero = SDL_SCANCODE_KP_0,

			ArrowRight = SDL_SCANCODE_RIGHT,
			ArrowUp = SDL_SCANCODE_UP,
			ArrowLeft = SDL_SCANCODE_LEFT,
			ArrowDown = SDL_SCANCODE_DOWN,

			// Aliases for dummy VR controllers.
			LeftTrigger = One,
			LeftPad = Two,
			LeftGrip = Three,
			RightTrigger = NumOne,
			RightGrip = NumTwo,
			RightPad = NumThree
		};

		BS_NON_COPYABLE(SdlManager);
		BS_NON_MOVEABLE(SdlManager);

		SdlManager();
		~SdlManager();

		void RegisterCallback(Input input, std::function<void(void)> callback);
		bool Update(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerPos, const float headAltitude);

		constexpr static uint32_t const DISPLAY_SIZE = 720;

	private:
		void DrawSourceAndListener_(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerPos, const float headAltitude);

		SDL_Event sdlEvent_{};
		SDL_Window* sdlWindow_ = nullptr;
		SDL_Renderer* sdlRenderer_ = nullptr;

		std::map<Input, std::vector<std::function<void(void)>>> callbacks_;
	};
}