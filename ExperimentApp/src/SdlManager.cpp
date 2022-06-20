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
	sdlWindow_ = SDL_CreateWindow("MyWindow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 720, 720, SDL_WINDOW_SHOWN);
	assert(sdlWindow_, "Failed to create sdl window!");
}

bsExp::SdlManager::~SdlManager()
{
	SDL_Quit();
}

void bsExp::SdlManager::RegisterCallback(Input input, std::function<void(void)> callback)
{
	callbacks_[input].push_back(callback);
}

bool bsExp::SdlManager::Update()
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

	return false;
}