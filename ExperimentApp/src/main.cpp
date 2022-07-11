#include "Application.h"

#define BSEXP_SEED 0x917774

int main()
{
	auto app = new bsExp::Application(BSEXP_SEED); // Allocated on heap due to structure size apparently exceeding stack.
	const auto result = app->RunProgram();
	delete app;
	return result;
}