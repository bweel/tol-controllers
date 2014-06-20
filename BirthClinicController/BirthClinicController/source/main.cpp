#include "BirthClinicController.h"


int main(int argc, char **argv)
{
    BirthClinicController* controller = new BirthClinicController();
    controller->run();
    delete controller;
    return 0;
}
