#pragma once

#include <random>

#include "BSCommon.h"

namespace bsExp
{
	class RandomEngine
	{
	public:
		BS_NON_COPYABLE(RandomEngine);
		BS_NON_MOVEABLE(RandomEngine);

		RandomEngine() = delete;
		RandomEngine(const size_t seed,
					 const float minSoundDist, const float maxSoundDist,
					 const float minSoundElevation, const float maxSoundElevation,
					 const float minSoundAzimuth, const float maxSoundAzimuth,
					 const size_t minRendererIndex, const size_t maxRendererIndex):
			seed(seed),
			e_(std::default_random_engine((unsigned int)seed)),
			distrAzimuth_(seed, minSoundAzimuth, maxSoundAzimuth),
			distrElevation_(seed, minSoundElevation, maxSoundElevation),
			distrRadius_(seed, minSoundDist, maxSoundDist),
			distrMiddleware_(seed, minRendererIndex, maxRendererIndex)
		{
		};

		bs::CartesianCoord GenCartesian(const float headAltitude);
		bs::SphericalCoord GenSpherical();
		float GenAzimuth();
		float GenElevation();
		float GenRadius();
		size_t GenMiddleware();

		const size_t seed;

	private:
		class UniDistrFloatGen_
		{
		public:
			UniDistrFloatGen_() = delete;
			UniDistrFloatGen_(const size_t seed, const float min, const float max):
				seed(seed), d_(std::uniform_real_distribution<float>(min, max))
			{
			};

			float Generate(std::default_random_engine& e);

			const size_t seed;

		private:
			std::uniform_real_distribution<float> d_;
		};

		class UniDistrUintGen_
		{
		public:
			UniDistrUintGen_() = delete;
			UniDistrUintGen_(const size_t seed, const size_t min, const size_t max): seed(seed), d_(std::uniform_int_distribution(min, max)) {};

			size_t Generate(std::default_random_engine& e);

			const size_t seed;

		private:
			std::uniform_int_distribution<size_t> d_;
		};

		UniDistrFloatGen_ distrAzimuth_, distrElevation_, distrRadius_;
		UniDistrUintGen_ distrMiddleware_;
		std::default_random_engine e_; // Important to have a single engine for generating all values, else it introduces patterns.
	};
}