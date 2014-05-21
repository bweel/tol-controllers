#include "Module.h"

Module::Module(id_t id, double arenaSize) : id(id), assembled(false), arenaSize(arenaSize), inReserve(true)
{
    //nix
}


/*void Module::setAssembled(bool value)
{
    assembled = value;
}*/


/*bool Module::getAssembled() const
{
    return assembled;
}*/


id_t Module::getId() const
{
    return id;
}


bool Module::getInReserve() const
{
    return inReserve;
}


void Module::toReserve()
{
    inReserve = true;
}


void Module::removeFromReserve()
{
    inReserve = false;
}


std::string Module::positionToString() const
{
    return std::string(getPosition().toString());;
}