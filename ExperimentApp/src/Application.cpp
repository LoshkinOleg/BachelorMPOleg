#include <Application.h>

bsExp::Application::Application(const size_t randSeed):
	rndEngine_(RandomEngine(randSeed, RendererManager::MIN_SOUND_DISTANCE, RendererManager::MAX_SOUND_DISTANCE, RendererManager::MIN_SOUND_ELEVATION, RendererManager::MAX_SOUND_ELEVATION, RendererManager::MIN_SOUND_AZIMUTH, RendererManager::MAX_SOUND_AZIMUTH, RendererManager::MIN_RENDERER_INDEX, RendererManager::MAX_RENDERER_INDEX))
{
	sdlManager_.RegisterCallback(SdlManager::Input::NumZero, [this]() { rendererManager_.StopAll(); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumOne, [this]() { rendererManager_.PlaySound("noise"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumTwo, [this]() { rendererManager_.PlaySound("speech"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumFour, [this]() { rendererManager_.PauseSound("noise"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumFive, [this]() { rendererManager_.PauseSound("speech"); });
	sdlManager_.RegisterCallback(SdlManager::Input::NumPlus, [this]() { SetRandomSourcePos_(); });

	sdlManager_.RegisterCallback(SdlManager::Input::ArrowUp, [this]() { sourceTransform_.m03 += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowDown, [this]() { sourceTransform_.m03 -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowLeft, [this]() { sourceTransform_.m13 += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::ArrowRight, [this]() { sourceTransform_.m13 -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumMultiply, [this]() { sourceTransform_.m23 += 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumDivide, [this]() { sourceTransform_.m23 -= 0.1f; });
	sdlManager_.RegisterCallback(SdlManager::Input::NumMinus, [this]() { logger_.LogNewSoundPos(sourceTransform_.GetPosition()); });
	sdlManager_.RegisterCallback(SdlManager::Input::Spacebar, [this]() { SelectRandomRenderer_(); });

#ifdef BUILD_WITH_OPENVR
	sdlManager_.RegisterCallback(SdlManager::Input::Backspace, [this]()
	{
		logger_.LogHeadSetPosAndRot(openVrManager_.GetHeadsetPos(), openVrManager_.GetHeadsetRot().GetEuler());
	});
	sdlManager_.RegisterCallback(SdlManager::Input::One, [this]() { OnLeftTrigger_(); });
	sdlManager_.RegisterCallback(SdlManager::Input::Two, [this]() { OnRightTrigger_(); });

	openVrManager_.RegisterCallback(OpenVrManager::Input::LeftTrigger, [this]() { OnLeftTrigger_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightTrigger, [this]() { OnRightTrigger_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightGrip, [this]() { OnRightGrip_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPadRight, [this]() { OnRightPadRight_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightMenu, [this]() { OnRightMenu_(); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPadRight, [this]() { logger_.LogMsg("Right pad right."); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPadUp, [this]() { logger_.LogMsg("Right pad up."); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPadLeft, [this]() { logger_.LogMsg("Right pad left."); });
	openVrManager_.RegisterCallback(OpenVrManager::Input::RightPadDown, [this]() { logger_.LogMsg("Right pad down."); });
#endif
	SetRandomSourcePos_();
	SelectRandomRenderer_();
}

int bsExp::Application::RunProgram()
{
	bool shutdown = false;
	while (!shutdown)
	{
		rendererManager_.CallImplementationsUpdates();
#ifdef BUILD_WITH_OPENVR
		openVrManager_.Update();
		rendererManager_.MoveAllSounds(sourceTransform_.GetPosition());
		rendererManager_.SetListenerTransform(openVrManager_.GetHeadsetMat());
		shutdown = sdlManager_.Update(sourceTransform_, openVrManager_.GetHeadsetMat(), openVrManager_.GetLeftControllerMat(), RendererManager::HEAD_ALTITUDE, rendererManager_.AnyPlaying());
#else
		rendererManager_.MoveAllSounds(sourceTransform_.GetPosition());
		rendererManager_.SetListenerTransform(bs::Mat3x4());
		shutdown = sdlManager_.Update(sourceTransform_, bs::Mat3x4(), bs::Mat3x4(), RendererManager::HEAD_ALTITUDE, rendererManager_.AnyPlaying());
#endif
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
	const auto pos = rndEngine_.GenCartesian();
	sourceTransform_.m03 = pos.x;
	sourceTransform_.m13 = pos.y;
	sourceTransform_.m23 = pos.z + rendererManager_.HEAD_ALTITUDE;
	logger_.LogNewSoundPos({ sourceTransform_.m03 , sourceTransform_.m13 , sourceTransform_.m23 });
}

void bsExp::Application::OnLeftTrigger_()
{
#ifdef BUILD_WITH_OPENVR
	logger_.LogControllerPosition("Participant controller", openVrManager_.GetLeftControllerPos());
#else
	logger_.LogControllerPosition("Participant controller", bs::CartesianCoord());
#endif
}

void bsExp::Application::OnRightTrigger_()
{
#ifdef BUILD_WITH_OPENVR
	logger_.LogControllerPosition("Conductor controller", openVrManager_.GetRightControllerPos());
#else
	logger_.LogControllerPosition("Conductor controller", bs::CartesianCoord());
#endif
}

void bsExp::Application::OnRightGrip_()
{
	if (rendererManager_.IsPaused("noise"))
	{
		rendererManager_.PlaySound("noise");
		// logger_.LogNoiseToggle(true);
	}
	else
	{
		rendererManager_.PauseSound("noise");
		// logger_.LogNoiseToggle(false);
	}
}

void bsExp::Application::OnRightPadRight_()
{
	SelectRandomRenderer_();
}

void bsExp::Application::OnRightPadLeft_()
{
}

void bsExp::Application::OnRightMenu_()
{
	logger_.LogDelimiter();
}
