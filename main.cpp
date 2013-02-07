
#include "all.h"
#include "command.h"

using namespace main_loop;

int main()
{
	init();
	loop();
	destroy();

	return 0;
}

