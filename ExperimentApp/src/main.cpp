#include "Application.h"

#include <time.h>

int main()
{
	auto app = new bsExp::Application(BSEXP_SEED == 0 ? std::time(0) : BSEXP_SEED); // Allocated on heap due to structure size apparently exceeding stack.
	const auto result = app->RunProgram();
	delete app;
	return result;
}