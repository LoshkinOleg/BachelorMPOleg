#include <Application.h>

bsExp::Application::Application(const size_t randSeed):
	rndEngine_(RandomEngine(randSeed, RendererManager::MIN_SOUND_DISTANCE, RendererManager::MAX_SOUND_DISTANCE, RendererManager::MIN_SOUND_ELEVATION, RendererManager::MAX_SOUND_ELEVATION, RendererManager::MIN_SOUND_AZIMUTH, RendererManager::MAX_SOUND_AZIMUTH, RendererManager::MIN_RENDERER_INDEX, RendererManager::MAX_RENDERER_INDEX))
{
	sdlManager_.RegisterCallback(SdlManager::Input::NumZero, [this]() { rendererManager_.StopAll(); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumOne, [this]() { rendererManager_.PlaySound("noise"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumTwo, [this]() { rendererManager_.PlaySound("speech"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumThree, [this]() { rendererManager_.PlaySound("squeak"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumFour, [this]() { rendererManager_.PauseSound("noise"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumFive, [this]() { rendererManager_.PauseSound("speech"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumSix, [this]() { rendererManager_.PauseSound("squeak"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumPlus, [this]() { SetRandomSourcePos_(); });

	sdlManager_.RegisterCallback(SdlManager::Input::ArrowUp, [this]() { sourceTransform_.m03 += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowDown, [this]() { sourceTransform_.m03 -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowLeft, [this]() { sourceTransform_.m13 += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowRight, [this]() { sourceTransform_.m13 -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumMultiply, [this]() { sourceTransform_.m23 += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumDivide, [this]() { sourceTransform_.m23 -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumMinus, [this]() { logger_.LogNewSoundPos(sourceTransform_.GetPosition()); });
	sdlManager_.RegisterCallback(SdlManager::Input::Backspace, [this]()
	{
		logger_.LogHeadSetPosAndRot(openVrManager_.GetHeadsetPos(), openVrManager_.GetHeadsetRot().GetEuler());
	});

	sdlManager_.RegisterCallback(SdlManager::Input::Spacebar, [this]() { SelectRandomRenderer_(); });

	sdlManager_.RegisterCallback(SdlManager::Input::One, [this]() { OnLeftTrigger_(); });
	sdlManager_.RegisterCallback(SdlManager::Input::Two, [this]() { OnRightTrigger_(); });

	openVrManager_.RegisterCallback(OpenVrManager::Input::LeftTrigger, [this]() { OnLeftTrigger_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightTrigger, [this]() { OnRightTrigger_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightGrip, [this]() { OnRightGrip_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPad, [this]() { OnRightPad_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightMenu, [this]() { OnRightMenu_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPadRight, [this]() { rendererManager_.PlaySound("speech"); });

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
		rendererManager_.MoveAllSounds(sourceTransform_.GetPosition());
		rendererManager_.SetListenerTransform(openVrManager_.GetHeadsetMat());
		shutdown = sdlManager_.Update(sourceTransform_, openVrManager_.GetHeadsetMat(), RendererManager::HEAD_ALTITUDE);
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
	const auto pos = rndEngine_.GenCartesian(rendererManager_.HEAD_ALTITUDE);
	sourceTransform_.m03 = pos.x;
	sourceTransform_.m13 = pos.y;
	sourceTransform_.m23 = pos.z;
	logger_.LogNewSoundPos(pos);
}

void bsExp::Application::OnLeftTrigger_()
{
	logger_.LogControllerPosition("Participant controller", openVrManager_.GetLeftControllerPos());
}

void bsExp::Application::OnRightTrigger_()
{
	logger_.LogControllerPosition("Conductor controller", openVrManager_.GetRightControllerPos());
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
