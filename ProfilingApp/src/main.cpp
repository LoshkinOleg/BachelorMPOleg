#include "Application.h"

int main()
{
	auto app = bsExp::Application(1337);
	return app.RunProgram();
}