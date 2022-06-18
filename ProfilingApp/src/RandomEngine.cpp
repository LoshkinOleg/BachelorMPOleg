#include "RandomEngine.h"

bs::CartesianCoord bsExp::RandomEngine::GenCartesian(const float headAltitude)
{
	// Oleg@self: fix distrubutions, creating incoherent positions.
	auto c = bs::ToCartesian(GenSpherical());
	c.z += headAltitude;
	return c;
}

bs::SphericalCoord bsExp::RandomEngine::GenSpherical()
{
	return { GenAzimuth(), GenElevation(), GenRadius() };
}

float bsExp::RandomEngine::GenAzimuth()
{
	return distrAzimuth_.Generate();
}

float bsExp::RandomEngine::GenElevation()
{
	return distrElevation_.Generate();
}

float bsExp::RandomEngine::GenRadius()
{
	return distrRadius_.Generate();
}

size_t bsExp::RandomEngine::GenMiddleware()
{
	return distrMiddleware_.Generate();
}

float bsExp::RandomEngine::UniDistrFloatGen_::Generate()
{
	return d_(e_);
}

size_t bsExp::RandomEngine::UniDistrUintGen_::Generate()
{
	return d_(e_);
}