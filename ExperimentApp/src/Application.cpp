#include <Application.h>

bsExp::Application::Application(const size_t randSeed):
	rndEngine_(RandomEngine(randSeed, RendererManager::MIN_SOUND_DISTANCE, RendererManager::MAX_SOUND_DISTANCE, RendererManager::MIN_SOUND_ELEVATION, RendererManager::MAX_SOUND_ELEVATION, RendererManager::MIN_SOUND_AZIMUTH, RendererManager::MAX_SOUND_AZIMUTH, RendererManager::MIN_RENDERER_INDEX, RendererManager::MAX_RENDERER_INDEX))
{
	sdlManager_.RegisterCallback(SdlManager::Input::NumZero, [this]() { rendererManager_.StopAll(); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumOne, [this]() { rendererManager_.PlaySound("noise"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumTwo, [this]() { rendererManager_.PlaySound("speech"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumThree, [this]() { rendererManager_.PlaySound("sweep"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumFour, [this]() { rendererManager_.PauseSound("noise"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumFive, [this]() { rendererManager_.PauseSound("speech"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumSix, [this]() { rendererManager_.PauseSound("sweep"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumPlus, [this]() { SetRandomSourcePos_(); });

	sdlManager_.RegisterCallback(SdlManager::Input::ArrowUp, [this]() { currentSoundPos_.x += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowDown, [this]() { currentSoundPos_.x -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowLeft, [this]() { currentSoundPos_.y += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowRight, [this]() { currentSoundPos_.y -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumMultiply, [this]() { currentSoundPos_.z += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumDivide, [this]() { currentSoundPos_.z -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumMinus, [this]() { logger_.LogNewSoundPos(currentSoundPos_); });

	sdlManager_.RegisterCallback(SdlManager::Input::Spacebar, [this]() { SelectRandomRenderer_(); });

	sdlManager_.RegisterCallback(SdlManager::Input::One, [this]() { OnLeftTrigger_(); });
	sdlManager_.RegisterCallback(SdlManager::Input::Two, [this]() { OnRightTrigger_(); });

	openVrManager_.RegisterCallback(OpenVrManager::Input::LeftTrigger, [this]() { OnLeftTrigger_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightTrigger, [this]() { OnRightTrigger_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightGrip, [this]() { OnRightGrip_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPad, [this]() { OnRightPad_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightMenu, [this]() { OnRightMenu_(); });

	SetRandomSourcePos_();
	SelectRandomRenderer_();
}

int bsExp::Application::RunProgram()
{
	bool shutdown = false;
	while (!shutdown)
	{
		rendererManager_.CallImplementationsUpdates();
		openVrManager_.Update();
		leftControllerPos_ = openVrManager_.LeftControllerPos();
		rightControllerPos_ = openVrManager_.RightControllerPos();
		rendererManager_.MoveAllSounds(currentSoundPos_);
		shutdown = sdlManager_.Update(currentSoundPos_, {0.0f, 0.0f, RendererManager::HEAD_ALTITUDE}, RendererManager::HEAD_ALTITUDE);
	}

	return 0;
}

void bsExp::Application::SelectRandomRenderer_()
{
	const auto selectedRenderer = (RendererManager::AudioRendererType)rndEngine_.GenMiddleware();
	rendererManager_.SetSelectedRenderer(selectedRenderer);
	switch (selectedRenderer)
	{
		case RendererManager::AudioRendererType::ThreeDTI:
		{
			logger_.LogRendererChange("3dti");
		}
		break;
		case RendererManager::AudioRendererType::Fmod:
		{
			logger_.LogRendererChange("fmod");
		}
		break;
		default:
		break;
	}
}

void bsExp::Application::SetRandomSourcePos_()
{
	currentSoundPos_ = rndEngine_.GenCartesian(rendererManager_.HEAD_ALTITUDE);
	logger_.LogNewSoundPos(currentSoundPos_);
}

void bsExp::Application::OnLeftTrigger_()
{
	logger_.LogControllerPosition("Participant controller", leftControllerPos_);
}

void bsExp::Application::OnRightTrigger_()
{
	logger_.LogControllerPosition("Scientist controller", rightControllerPos_);
}

void bsExp::Application::OnRightGrip_()
{
	if (rendererManager_.IsPaused("noise"))
	{
		rendererManager_.PlaySound("noise");
		logger_.LogNoiseToggle(true);
	}
	else
	{
		rendererManager_.PauseSound("noise");
		logger_.LogNoiseToggle(false);
	}
}

void bsExp::Application::OnRightPad_()
{
	SelectRandomRenderer_();
}

void bsExp::Application::OnRightMenu_()
{
	logger_.LogDelimiter();
}
