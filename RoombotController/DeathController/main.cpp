#include "DeathController.h"

int main(int argc, const char * argv[])
{
	DeathController * robot = new DeathController();
	robot->run();
	delete(robot);
}