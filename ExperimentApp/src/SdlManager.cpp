#include "SdlManager.h"

#include <cassert>
#include <functional>
#include <tuple>

bsExp::SdlManager::SdlManager()
{
	if (SDL_Init(SDL_INIT_EVERYTHING))
	{
		assert(false, "Failed to initialize SDL!");
	}
	if (SDL_CreateWindowAndRenderer(DISPLAY_WIDTH, DISPLAY_HEIGHT, SDL_WINDOW_SHOWN, &sdlWindow_, &sdlRenderer_))
	{
		assert(sdlWindow_, "Failed to create sdl window and renderer!");
	}
}

bsExp::SdlManager::~SdlManager()
{
	assert(sdlRenderer_ && sdlWindow_, "SDL renderer or window is nullptr.");
	SDL_DestroyRenderer(sdlRenderer_);
	SDL_DestroyWindow(sdlWindow_);
	SDL_Quit();
}

void bsExp::SdlManager::RegisterCallback(Input input, std::function<void(void)> callback)
{
	callbacks_[input].push_back(callback);
}

bool bsExp::SdlManager::Update(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerCoord)
{
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

void bsExp::SdlManager::DrawSourceAndListener_(const bs::CartesianCoord sourcePos, const bs::CartesianCoord listenerCoord)
{
	auto draw3x3 = [this](const bs::CartesianCoord coord, const int minX, const int maxX, const int minY, const int maxY)
	{
		SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + (maxX - minX) / 2, (int)(coord.y * 100) + (maxY - minY) / 2);
		SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + ((maxX - minX) / 2) - 1, (int)(coord.y * 100) + (maxY - minY) / 2);
		SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + ((maxX - minX) / 2) + 1, (int)(coord.y * 100) + (maxY - minY) / 2);
		SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + (maxX - minX) / 2, (int)(coord.y * 100) + ((maxY - minY) / 2) - 1);
		SDL_RenderDrawPoint(sdlRenderer_, (int)(coord.x * 100) + (maxX - minX) / 2, (int)(coord.y * 100) + ((maxY - minY) / 2) + 1);
	};

	// Clear screen.
	SDL_SetRenderDrawColor(sdlRenderer_, 0, 0, 0, 255);
	SDL_RenderClear(sdlRenderer_);

	// Draw viewport separators.
	SDL_SetRenderDrawColor(sdlRenderer_, 255, 255, 255, 255);
	SDL_RenderDrawLine(sdlRenderer_, DISPLAY_WIDTH / 2, 0, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT);
	SDL_RenderDrawLine(sdlRenderer_, 0, DISPLAY_HEIGHT / 2, DISPLAY_WIDTH, DISPLAY_HEIGHT / 2);

	// Draw x,y positions.
	int minX = 0;
	int maxX = DISPLAY_WIDTH / 2;
	int minY = 0;
	int maxY = DISPLAY_HEIGHT / 2;
	SDL_SetRenderDrawColor(sdlRenderer_, 255, 0, 0, 255);
	draw3x3(sourcePos, minX, maxX, minY, maxY);
	SDL_SetRenderDrawColor(sdlRenderer_, 0, 255, 0, 255);
	draw3x3(listenerCoord, minX, maxX, minY, maxY);

	// Present.
	SDL_RenderPresent(sdlRenderer_);
}
