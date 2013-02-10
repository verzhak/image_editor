 
#include <cstring>
#include <unistd.h>
#include <HsFFI.h>

using namespace std;

static int pid;
 
extern "C"
{
	static void init(void) __attribute__((constructor));
	static void exit(void) __attribute__((destructor));
}

static void init()
{
    static char ** argv = new char *[2];
    static int argc = 1;

	argv[0] = new char[strlen("libamv_morphology.so") + 1];
	strcpy(argv[0], "libamv_morphology.so");
	argv[1] = 0;

	pid = getpid();

    hs_init(& argc, & argv);
}
 
static void exit()
{
	if(pid == getpid())
		hs_exit();
}

