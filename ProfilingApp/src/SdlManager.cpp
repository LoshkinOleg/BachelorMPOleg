#include "SdlManager.h"

#include <cassert>

bsExp::SdlManager::SdlManager(const std::function<void(void)>& leftTriggerCallback,
							  const std::function<void(void)>& rightTriggerCallback,
							  const std::function<void(void)>& leftGripCallback,
							  const std::function<void(void)>& leftPadCallback,
							  const std::function<void(void)>& escapeCallback):
	leftTriggerCallback_(leftTriggerCallback),
	rightTriggerCallback_(rightTriggerCallback),
	leftGripCallback_(leftGripCallback),
	leftPadCallback_(leftPadCallback),
	escapeCallback_(escapeCallback)
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

bool bsExp::SdlManager::Update()
{
	constexpr const auto PARTICIPANT_TRIGGER = SDL_SCANCODE_1;
	constexpr const auto SCIENTIST_TRIGGER = SDL_SCANCODE_2;
	constexpr const auto TOGGLE_NOISE = SDL_SCANCODE_3;
	constexpr const auto CHANGE_MIDDLEWARE_AND_POS = SDL_SCANCODE_5;

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
						escapeCallback_();
						return true;
					}break;

					case PARTICIPANT_TRIGGER:
					{
						rightTriggerCallback_();
					}break;

					case SCIENTIST_TRIGGER:
					{
						leftTriggerCallback_();
					}break;

					case TOGGLE_NOISE:
					{
						leftGripCallback_();
					}break;

					case CHANGE_MIDDLEWARE_AND_POS:
					{
						leftPadCallback_();
					}break;

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