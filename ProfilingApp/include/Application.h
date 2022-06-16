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
		BSEXP_NON_COPYABLE(Application);
		BSEXP_NON_MOVEABLE(Application);

		Application() = delete;
		Application(const char* hrtfFile, const char* brirFile, const char* soundFile, const size_t randSeed);
		~Application();

		int RunProgram();

	private:
		void UpdateTransforms_();
		void SetRandomSourcePos_();

	private:
		SdlManager sdlManager_;
		RendererManager rendererManager_;
		RandomEngine rndManager_;
		Logger logger_;

		bs::CartesianCoord currentSoundPos_{};
	};
}