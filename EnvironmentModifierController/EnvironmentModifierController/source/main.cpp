#include "EnvironmentModifierController.h"

int main(int argc, char **argv)
{
    EnvironmentModifierController * controller = new EnvironmentModifierController();
    controller->run();
    delete controller;
    return 0;
}
