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

bool bsExp::SdlManager::Update(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerCoord)
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

	DrawSourceAndListener_(sourcePos, listenerCoord);

	return false;
}

void bsExp::SdlManager::DrawSourceAndListener_(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerPos)
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
		int min = 0;
		int max = DISPLAY_SIZE / 2;

		const auto draw3x3crossXy = [this](const bs::CartesianCoord coord, const int min, const int max)
		{
			SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + (max - min) / 2, (int)(coord.y * 100) + (max - min) / 2);
			SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + ((max - min) / 2) - 1, (int)(coord.y * 100) + (max - min) / 2);
			SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + ((max - min) / 2) + 1, (int)(coord.y * 100) + (max - min) / 2);
			SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + (max - min) / 2, (int)(coord.y * 100) + ((max - min) / 2) - 1);
			SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + (max - min) / 2, (int)(coord.y * 100) + ((max - min) / 2) + 1);
		};
		SDL_SetRenderDrawColor(sdlRenderer_, 255, 0, 0, 255);
		const bs::CartesianCoord adjustedSourcePos = {-sourcePos.y, -sourcePos.x, sourcePos.z};
		draw3x3crossXy(adjustedSourcePos, min, max);
		SDL_SetRenderDrawColor(sdlRenderer_, 0, 255, 0, 255);
		draw3x3crossXy(listenerPos, min, max);

		// Present.
		SDL_RenderPresent(sdlRenderer_);
	}

	lastSourcePos = sourcePos;
	lastListenerPos = listenerPos;
}
