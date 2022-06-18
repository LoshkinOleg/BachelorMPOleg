#include "RandomEngine.h"

bs::CartesianCoord bsExp::RandomEngine::GenCartesian()
{
	// Oleg@self: fix distrubutions, creating incoherent positions.
	return { bs::ToCartesian(GenSpherical()) };
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

float bsExp::RandomEngine::NormDistrFloatGen_::Generate()
{
	return d_(e_);
}

size_t bsExp::RandomEngine::UniDistrUintGen_::Generate()
{
	return d_(e_);
}