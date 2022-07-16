#pragma once

#include "SdlManager.h"
#include "RendererManager.h"
#include "RandomEngine.h"
#include "Logger.h"
#include "OpenVrManager.h"

namespace bsExp
{
	class Application
	{
	public:
		BS_NON_COPYABLE(Application);
		BS_NON_MOVEABLE(Application);

		Application() = delete;
		Application(const size_t randSeed);

		int RunProgram();

	private:
		void SelectRandomRenderer_();
		void SetRandomSourcePos_();

		void OnLeftTrigger_();

		void OnRightTrigger_();
		void OnRightGrip_();
		void OnRightMenu_();
		void OnRightPadRight_();
		void OnRightPadLeft_();

		RendererManager rendererManager_{};
		SdlManager sdlManager_{};
#ifdef BUILD_WITH_OPENVR
		OpenVrManager openVrManager_{};
#endif
		Logger logger_{};
		RandomEngine rndEngine_;

		bs::Mat3x4 sourceTransform_{};
	};
}