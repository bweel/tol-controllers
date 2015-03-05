#include "RoombotController.h"
#include <sys/resource.h>
#include <iostream>


bool EnableCoreDumps()
{
    struct rlimit   limit;
    
    limit.rlim_cur = RLIM_INFINITY;
    limit.rlim_max = RLIM_INFINITY;
    return setrlimit(RLIMIT_CORE, &limit) == 0;
}

int main(int arg_c, char ** arg_v)
{
//    EnableCoreDumps();
	RoombotController robot;
	robot.run();
}