#include <Application.h>

bsExp::Application::Application(const size_t randSeed):
	rndEngine_(RandomEngine(randSeed, RendererManager::MIN_SOUND_DISTANCE, RendererManager::MAX_SOUND_DISTANCE, RendererManager::MIN_SOUND_ELEVATION, RendererManager::MAX_SOUND_ELEVATION, RendererManager::MIN_SOUND_AZIMUTH, RendererManager::MAX_SOUND_AZIMUTH, RendererManager::MIN_RENDERER_INDEX, RendererManager::MAX_RENDERER_INDEX))
{
	sdlManager_.RegisterCallback(SdlManager::Input::LeftTrigger, [this]() { OnLeftTrigger_(); });
	sdlManager_.RegisterCallback(SdlManager::Input::RightTrigger, [this]() { OnRightTrigger_(); });
	sdlManager_.RegisterCallback(SdlManager::Input::RightGrip, [this]() { OnRightGrip_(); });
	sdlManager_.RegisterCallback(SdlManager::Input::RightPad, [this]() { OnRightPad_(); });

	SetRandomSourcePos_();
	SelectRandomRenderer_();
}

int bsExp::Application::RunProgram()
{
	bool shutdown = false;
	while (!shutdown)
	{
		UpdateTransforms_();
		rendererManager_.MoveAllSounds(currentSoundPos_);
		rendererManager_.CallImplementationsUpdates();
		shutdown = sdlManager_.Update();
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
		default:
		break;
	}
}

void bsExp::Application::UpdateTransforms_()
{
	// Dummy implementation, move openvr code here to retireve controller positions.
	leftControllerPos = rndEngine_.GenCartesian();
	rightControllerPos = rndEngine_.GenCartesian();
}

void bsExp::Application::SetRandomSourcePos_()
{
	currentSoundPos_ = rndEngine_.GenCartesian();
	logger_.LogNewSoundPos(currentSoundPos_);
}

void bsExp::Application::OnLeftTrigger_()
{
	logger_.LogControllerPosition("Participant controller", leftControllerPos);
}

void bsExp::Application::OnRightTrigger_()
{
	logger_.LogControllerPosition("Scientist controller", rightControllerPos);
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
