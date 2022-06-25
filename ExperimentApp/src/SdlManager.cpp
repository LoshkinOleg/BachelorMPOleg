#include "SdlManager.h"

#include <cassert>
#include <functional>
#include <tuple>

#include <easy/profiler.h>

bsExp::SdlManager::SdlManager()
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		assert(false, "Failed to initialize SDL!");
	}
	if (SDL_CreateWindowAndRenderer(DISPLAY_SIZE, DISPLAY_SIZE, SDL_WINDOW_SHOWN, &sdlWindow_, &sdlRenderer_))
	{
		assert(sdlWindow_, "Failed to create sdl window and renderer!");
	}
	assert(sdlWindow_, "Failed to create sdl window!");
}

bsExp::SdlManager::~SdlManager()
{
	if(sdlRenderer_)SDL_DestroyRenderer(sdlRenderer_);
	if(sdlWindow_)SDL_DestroyWindow(sdlWindow_);
	SDL_Quit();
}

void bsExp::SdlManager::RegisterCallback(Input input, std::function<void(void)> callback)
{
	callbacks_[input].push_back(callback);
}

bool bsExp::SdlManager::Update(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerPos, const float headAltitude)
{
	EASY_FUNCTION("SdlManager::Update");

	while (SDL_PollEvent(&sdlEvent_))
	{
		switch (sdlEvent_.type)
		{
			case SDL_KEYDOWN:
			{
				if (sdlEvent_.key.keysym.scancode == SDL_SCANCODE_ESCAPE) // Key reserved for exiting the program.
				{
					return true;
				}
				else
				{
					const auto& callbacks = callbacks_[(Input)sdlEvent_.key.keysym.scancode];
					for (auto& callback : callbacks)
					{
						callback();
					}
				}
			}
			break;

			case SDL_QUIT:
			{
				return true;
			}
			break;

			default:
			break;
		}
	}

	DrawSourceAndListener_(sourcePos, listenerPos, headAltitude);

	return false;
}

void bsExp::SdlManager::DrawSourceAndListener_(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerPos, const float headAltitude)
{
	EASY_FUNCTION("SdlManager::DrawSourceAndListener_");

	static bs::CartesianCoord lastSourcePos{};
	static bs::CartesianCoord lastListenerPos{};

	if (!sdlRenderer_) return;

	if (!bs::Equivalent(sourcePos, lastListenerPos) || !bs::Equivalent(listenerPos, lastListenerPos)) // Avoid visual processing if there's no change.
	{
		EASY_BLOCK("Drawing on display.");

		// Clear screen.
		SDL_SetRenderDrawColor(sdlRenderer_, 0, 0, 0, 255);
		SDL_RenderClear(sdlRenderer_);

		// Draw viewport separators.
		SDL_SetRenderDrawColor(sdlRenderer_, 255, 255, 255, 255);
		SDL_RenderDrawLine(sdlRenderer_, DISPLAY_SIZE / 2, 0, DISPLAY_SIZE / 2, DISPLAY_SIZE);
		SDL_RenderDrawLine(sdlRenderer_, 0, DISPLAY_SIZE / 2, DISPLAY_SIZE, DISPLAY_SIZE / 2);

		// Draw positions.
		constexpr const int SCALE_FACTOR = 50;
		constexpr const int SQUARE_SIDE = 5;
		constexpr const int SQUARE_HALF_SIDE = 2;
		constexpr const int FRONT_DIR_INDICATOR_LEN = 5;
		int centerX = DISPLAY_SIZE / 4;
		int centerY = centerX;
		// const bs::CartesianCoord adjustedSourcePos = {-sourcePos.y, -sourcePos.x, sourcePos.z};
		// const bs::CartesianCoord adjustedListenerPos = {-listenerPos.y, -listenerPos.x, listenerPos.z};
		SDL_Rect r = { (int)(-sourcePos.y * SCALE_FACTOR) + centerX - SQUARE_HALF_SIDE, (int)(-sourcePos.x * SCALE_FACTOR) + centerY - SQUARE_HALF_SIDE, SQUARE_SIDE, SQUARE_SIDE };

		// Draw source position in red.
		SDL_SetRenderDrawColor(sdlRenderer_, 255, 0, 0, 255);
		SDL_RenderDrawRect(sdlRenderer_, &r);
		centerX += DISPLAY_SIZE / 2;
		r = { (int)(-sourcePos.y * SCALE_FACTOR) + centerX - SQUARE_HALF_SIDE, (int)((-sourcePos.z) * SCALE_FACTOR) + centerY - SQUARE_HALF_SIDE, SQUARE_SIDE, SQUARE_SIDE };
		SDL_RenderDrawRect(sdlRenderer_, &r);

		// Draw listener position in green.
		centerX = DISPLAY_SIZE / 4;
		centerY = centerX;
		r = { (int)(-listenerPos.y * SCALE_FACTOR) + centerX - SQUARE_HALF_SIDE, (int)(-listenerPos.x * SCALE_FACTOR) + centerY - SQUARE_HALF_SIDE, SQUARE_SIDE, SQUARE_SIDE };

		SDL_SetRenderDrawColor(sdlRenderer_, 0, 255, 0, 255);
		SDL_RenderDrawRect(sdlRenderer_, &r);
		centerX += DISPLAY_SIZE / 2;
		r = { (int)(-listenerPos.y * SCALE_FACTOR) + centerX - SQUARE_HALF_SIDE, (int)((-listenerPos.z) * SCALE_FACTOR) + centerY - SQUARE_HALF_SIDE, SQUARE_SIDE, SQUARE_SIDE };
		SDL_RenderDrawRect(sdlRenderer_, &r);

		// Present.
		SDL_RenderPresent(sdlRenderer_);
	}

	lastSourcePos = sourcePos;
	lastListenerPos = listenerPos;
}
