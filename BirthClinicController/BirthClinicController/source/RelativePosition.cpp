#include "RelativePosition.h"

RelativePosition::RelativePosition(int x, int z, bool isHorizontal) : x(x), z(z), isHorizontal(isHorizontal)
{
    //nix
}


int RelativePosition::nrOfAdjacentPositions(const RelativePosition& other) const
{
    int result =0;
    if(adjacent(getX(), getZ(), other.getX(), other.getZ())) result++;
    if(adjacent(getX2(), getZ2(), other.getX(), other.getZ())) result++;
    if(adjacent(getX(), getZ(), other.getX2(), other.getZ2())) result++;
    if(adjacent(getX2(), getZ2(), other.getX2(), other.getZ2())) result++;
    return result;
}


bool RelativePosition::adjacent(const RelativePosition& other) const
{
    if(adjacent(getX(), getZ(), other.getX(), other.getZ())) return true;
    if(adjacent(getX2(), getZ2(), other.getX(), other.getZ())) return true;
    if(adjacent(getX(), getZ(), other.getX2(), other.getZ2())) return true;
    if(adjacent(getX2(), getZ2(), other.getX2(), other.getZ2())) return true;
    return false;
}


int RelativePosition::nrOfAdjacentPositions(int x, int y) const
{
    int result =0;
    if(adjacent(x, y, getX(), getZ())) result++;
    if(adjacent(x, y, getX2(), getZ2())) result++;
    return result;
}


bool RelativePosition::adjacent(int firstX, int firstY, int secondX, int secondY)
{
    if(abs(firstX-secondX) + abs(firstY-secondY) == 1) return true;
    return false;
}


int RelativePosition::getX() const
{
    return x;
}


int RelativePosition::getZ() const
{
    return z;
}


int RelativePosition::getX2() const
{
    if(isHorizontal) return x+1;
    return x;
}


int RelativePosition::getZ2() const
{
    if(isHorizontal) return z;
    return z+1;
}


bool RelativePosition::operator<(const RelativePosition &other) const
{
    if(x<other.x) return true;
    if(x>other.x) return false;
    if(z<other.z) return true;
    if(z>other.z) return false;
    if(!other.isHorizontal || isHorizontal) return false;
    return true;
}


bool RelativePosition::operator==(const RelativePosition &other) const
{
    return (x==other.x && z == other.z && isHorizontal == other.isHorizontal);
}


bool RelativePosition::equals(const RelativePosition& other) const
{
    return (x==other.x && z == other.z && isHorizontal == other.isHorizontal);
}