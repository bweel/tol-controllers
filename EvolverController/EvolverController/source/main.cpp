#include "EvolverController.h"

int main(int argc, char **argv)
{
    EvolverController * controller = new EvolverController();
    controller->run();
    delete controller;
    return 0;
}
