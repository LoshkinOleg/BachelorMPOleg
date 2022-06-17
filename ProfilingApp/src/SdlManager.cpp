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
				switch (sdlEvent_.key.keysym.scancode)
				{
					case SDL_SCANCODE_ESCAPE:
					{
						return true;
					}
					break;

					case SDL_SCANCODE_1:
					{
						const auto& callbacks = callbacks_[Input::LeftTrigger];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_2:
					{
						const auto& callbacks = callbacks_[Input::RightTrigger];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_3:
					{
						const auto& callbacks = callbacks_[Input::LeftGrip];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_4:
					{
						const auto& callbacks = callbacks_[Input::RightGrip];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_5:
					{
						const auto& callbacks = callbacks_[Input::LeftPad];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_6:
					{
						const auto& callbacks = callbacks_[Input::RightPad];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_7:
					{
						const auto& callbacks = callbacks_[Input::Spacebar];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_8:
					{
						const auto& callbacks = callbacks_[Input::Enter];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					case SDL_SCANCODE_9:
					{
						const auto& callbacks = callbacks_[Input::Backspace];
						for (auto& callback : callbacks)
						{
							callback();
						}
					}
					break;

					default:
					break;
				}
			}break;

			case SDL_QUIT:
			{
				return true;
			}break;

			default:
			break;
		}
	}

	return false;
}