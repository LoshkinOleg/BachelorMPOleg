#include "RandomEngine.h"

bs::CartesianCoord bsExp::RandomEngine::GenCartesian(const float headAltitude)
{
	auto c = bs::CartesianCoord(GenSpherical());
	c.z += headAltitude;
	return c;
}

bs::SphericalCoord bsExp::RandomEngine::GenSpherical()
{
	return { GenAzimuth(), GenElevation(), GenRadius() };
}

float bsExp::RandomEngine::GenAzimuth()
{
	return distrAzimuth_.Generate(e_);
}

float bsExp::RandomEngine::GenElevation()
{
	return distrElevation_.Generate(e_);
}

float bsExp::RandomEngine::GenRadius()
{
	return distrRadius_.Generate(e_);
}

size_t bsExp::RandomEngine::GenMiddleware()
{
	return distrMiddleware_.Generate(e_);
}

float bsExp::RandomEngine::UniDistrFloatGen_::Generate(std::default_random_engine& e)
{
	return d_(e);
}

size_t bsExp::RandomEngine::UniDistrUintGen_::Generate(std::default_random_engine& e)
{
	return d_(e);
}