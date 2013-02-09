 
#include <HsFFI.h>
 
extern "C"
{
	static void init(void) __attribute__((constructor));
	static void exit(void) __attribute__((destructor));
}

static void init()
{
    static char * argv[] = { "libamv_morphology.so", 0 }, ** argv_ = argv;
    static int argc = 1;
 
    hs_init(& argc, & argv_);
}
 
static void exit()
{
    hs_exit();
}

