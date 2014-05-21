#include "RoombotController.h"

#include <iostream>

int main(int arg_c, char ** arg_v)
{
	RoombotController * robot = new RoombotController();
	robot->run();
	delete(robot);
}