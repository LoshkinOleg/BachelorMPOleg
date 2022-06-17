#pragma once

#include "SdlManager.h"
#include "RendererManager.h"
#include "RandomEngine.h"
#include "Logger.h"

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
		void UpdateTransforms_();
		void SetRandomSourcePos_();

		void OnLeftTrigger_();
		void OnRightTrigger_();
		void OnRightGrip_();
		void OnRightPad_();

		RendererManager rendererManager_{};
		SdlManager sdlManager_{};
		Logger logger_{};
		RandomEngine rndEngine_;

		bs::CartesianCoord currentSoundPos_{};
		bs::CartesianCoord leftControllerPos{};
		bs::CartesianCoord rightControllerPos{};
	};
}