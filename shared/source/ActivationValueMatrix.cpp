#include "ActivationValueMatrix.h"


Coordinate::Coordinate(int x, int y, double value) : found(false), x(x), y(y), value(value)
{
    
}


bool Coordinate::operator<(const Coordinate &other) const
{
    if(x < other.x) return true;
    if(x > other.x) return false;
    if(y < other.y) return true;
    return false;
}


ActivationValueMatrix::ActivationValueMatrix(boost::shared_ptr<const NEAT::GeneticIndividual> genome, int width, int height) : width(width), height(height)
{
    //Initialise size
    xModifier = ((double)width-1)/2;
    yModifier = ((double)height-1)/2;
    
    //Initialise the phenotype
    cppn = genome->spawnFastPhenotypeStack<double> ();
    cppn.setValue(INPUT_BIAS, 1);
    cppn.setValue(INPUT_X, 0);
    cppn.setValue(INPUT_Y, 0);
    cppn.setActivated(true);
    
    //Initialise the 'used' matrix
    unsigned int i, j;
    used = new matrix<bool> (width, height);
    
    for (i = 0; i < used->size1(); i++) {
        for (j = 0; j < used->size2(); j++) {
            used->insert_element(i, j, false);
        }
    }
}


ActivationValueMatrix::~ActivationValueMatrix()
{
    delete(used);
}


Coordinate ActivationValueMatrix::getMax() const
{
    Coordinate result;
    result.found = false;
    result.value = THRESHOLD;
    
    int x, y;
    
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            if (get(x, y) > result.value && !getUsed(x, y)) {
                result.value = get(x, y);
                result.x = x;
                result.y = y;
                result.found = true;
            }
        }
    }
    
    return result;
}


Coordinate ActivationValueMatrix::getMaxNeighbour(const Coordinate& coordinate) const
{
    Coordinate result;
    result.found = false;
    result.value = THRESHOLD;
    int x, y, i;
    int xMod[NEIGHBOURS] = {-1, 1, 0, 0};
    int yMod[NEIGHBOURS] = {0, 0, -1, 1};
    
    //Test all neighbours
    for (i=0; i < NEIGHBOURS; i++) {
        x = coordinate.x + xMod[i];
        y = coordinate.y + yMod[i];
        if (inBounds(x, y)){
            if (!getUsed(x, y) && get(x, y) > result.value) {
                result.value = get(x, y);
                result.x = x;
                result.y = y;
                result.found = true;
            }
        }
    }
    
    return result;
}



Coordinate ActivationValueMatrix::getMaxAdjacent() const
{
    Coordinate result;
    result.found = false;
    result.value = THRESHOLD;
    set<Coordinate>::iterator it;
    
    for (it=adjacentCoordinates.begin(); it!=adjacentCoordinates.end(); it++){
        if (get(*it) > result.value && !getUsed(*it)) {
            result.value = get(*it);
            result.x = (*it).x;
            result.y = (*it).y;
            result.found = true;
        }
    }
    
    return result;
}



void ActivationValueMatrix::setUsed(const Coordinate& coordinate, bool newUsed)
{
    used->insert_element(coordinate.x, coordinate.y, newUsed);
    adjacentCoordinates.erase(coordinate);
}


void ActivationValueMatrix::addModuleCoordinate(const Coordinate& coordinate)
{
    int x, y, i;
    int xMod[NEIGHBOURS] = {-1, 1, 0, 0};
    int yMod[NEIGHBOURS] = {0, 0, -1, 1};
    
    //Add all free neighbours
    for (i=0; i < NEIGHBOURS; i++) {
        x = coordinate.x + xMod[i];
        y = coordinate.y + yMod[i];
        if (inBounds(x, y)){
            if (!getUsed(x, y)) {
                adjacentCoordinates.insert(Coordinate(x, y));
            }
        }
    }
}


bool ActivationValueMatrix::getUsed(int x, int y) const
{
    return used->at_element(x, y);
}


bool ActivationValueMatrix::getUsed(const Coordinate& coordinate) const
{
    return getUsed(coordinate.x, coordinate.y);
}


double ActivationValueMatrix::get(int x, int y) const
{
    cppn.setValue(INPUT_X, x - xModifier);
    cppn.setValue(INPUT_Y, y - yModifier);
    cppn.update();
    return cppn.getValue(OUTPUT);
}


double ActivationValueMatrix::getOne(int x, int y) const
{
    if ( (x == 0 && y == 0) || (x == 1 && y==0) ) {
        return THRESHOLD + 1;
    }
    return -THRESHOLD;
}


double ActivationValueMatrix::get(const Coordinate& coordinate) const
{
    return get(coordinate.x, coordinate.y);
}


bool ActivationValueMatrix::inBounds(int x, int y) const
{
    return x>=0 &&
    x<width &&
    y>=0 &&
    y<height;
}