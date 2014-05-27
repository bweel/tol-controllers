#include "RoombotBuildPlan.h"


RoombotBuildPlan::RoombotBuildPlan(size_t gridSize) : gridSize(gridSize)
{
    
}


size_t RoombotBuildPlan::size() const
{
    return positions.size();
}


RelativePosition RoombotBuildPlan::getRelativePosition(size_t index) const
{
    return positions.at(index);
}


Position RoombotBuildPlan::getPosition(const Position& center, size_t index) const
{
    RelativePosition relativePosistion = positions.at(index);
    
    double x = (((double)relativePosistion.x)*MODULE_WIDTH) + center.getX();
    double z = (((double)relativePosistion.z)*MODULE_WIDTH) + center.getZ();
    
    return Position(x, center.getY(), z, relativePosistion.isHorizontal);
}


bool RoombotBuildPlan::getConnector(size_t index, size_t connectorIndex) const
{
    int* xMods;
    int* zMods;
    int hxMods[NR_OF_CONNECTORS];
    int hyMods[NR_OF_CONNECTORS];
    int vxMods[NR_OF_CONNECTORS];
    int vyMods[NR_OF_CONNECTORS];
    
    vxMods[SOUTH_CONNECTOR] = 0;
    vxMods[SOUTH_EAST_CONNECTOR] = -1;
    vxMods[SOUTH_WEST_CONNECTOR] = 1;
    vxMods[NORTH_EAST_CONNECTOR] = -1;
    vxMods[NORTH_WEST_CONNECTOR] = 1;
    vxMods[NORTH_CONNECTOR] = 0;
    
    vyMods[SOUTH_CONNECTOR] = -1;
    vyMods[SOUTH_EAST_CONNECTOR] = 0;
    vyMods[SOUTH_WEST_CONNECTOR] = 0;
    vyMods[NORTH_EAST_CONNECTOR] = 1;
    vyMods[NORTH_WEST_CONNECTOR] = 1;
    vyMods[NORTH_CONNECTOR] = 2;
    
    hxMods[SOUTH_CONNECTOR] = -1;
    hxMods[SOUTH_EAST_CONNECTOR] = 0;
    hxMods[SOUTH_WEST_CONNECTOR] = 0;
    hxMods[NORTH_EAST_CONNECTOR] = 1;
    hxMods[NORTH_WEST_CONNECTOR] = 1;
    hxMods[NORTH_CONNECTOR] = 2;
    
    hyMods[SOUTH_CONNECTOR] = 0;
    hyMods[SOUTH_EAST_CONNECTOR] = 1;
    hyMods[SOUTH_WEST_CONNECTOR] = -1;
    hyMods[NORTH_EAST_CONNECTOR] = 1;
    hyMods[NORTH_WEST_CONNECTOR] = -1;
    hyMods[NORTH_CONNECTOR] = 0;
    
    if(positions[index].isHorizontal){
        xMods = hxMods;
        zMods = hyMods;
    } else {
        xMods = vxMods;
        zMods = vyMods;
    }
    
    if(connectorIndex == SOUTH_BOT_CONNECTOR ||
       connectorIndex == SOUTH_TOP_CONNECTOR ||
       connectorIndex == NORTH_BOT_CONNECTOR ||
       connectorIndex == NORTH_TOP_CONNECTOR)
    {
        return false;
    }
    
    int x =	positions[index].getX() + xMods[connectorIndex];
    int z = positions[index].getZ() + zMods[connectorIndex];
    
    return occupied(x, z);
}


void RoombotBuildPlan::add(int x, int z, bool isHorizontal)
{
    positions.push_back(RelativePosition(x,z,isHorizontal));
}


int RoombotBuildPlan::numberOfNeighbours(const RelativePosition& position) const
{
    int result = 0;
    
    for(size_t i=0; i<positions.size(); i++){
        result+= positions.at(i).nrOfAdjacentPositions(position);
    }
    
    return result;
}


int RoombotBuildPlan::numberOfNeighbours(int x, int z, bool isHorizontal) const
{
    return numberOfNeighbours(RelativePosition(x,z,isHorizontal));
}


int RoombotBuildPlan::numberOfNeighbours(int x, int z) const
{
    int result = 0;
    
    for(size_t i=0; i<positions.size(); i++){
        result+= positions.at(i).nrOfAdjacentPositions(x, z);
    }
    
    return result;
}


bool RoombotBuildPlan::equalOrganisms(BuildPlan* const otherBuildPlan, bool ignoreRotation, bool ignoreMirror) const
{
    RoombotBuildPlan* otherRoombotBuildPlan = dynamic_cast<RoombotBuildPlan*>(otherBuildPlan);
    if(otherRoombotBuildPlan == NULL) return false; //Test if the other buildplan is actually a Roombot
    
    RoombotBuildPlan other = *otherRoombotBuildPlan;
    
    if(equals(other)) return true; //Test normal state
    if(ignoreRotation){
        other.turnNinetyDegreesClockwise();
        if(equals(other)) return true; //Test 90 degree state
        other.turnNinetyDegreesClockwise();
        if(equals(other)) return true; //Test 180 degree state
        other.turnNinetyDegreesClockwise();
        if(equals(other)) return true; //Test 270 degree state
        other.turnNinetyDegreesClockwise();
    }
    
    //Repeat for the mirror of other
    if(!ignoreMirror) return false;
    other.flipHorizontally();
    
    if(equals(other)) return true; //Test mirror state
    if(ignoreRotation){
        other.turnNinetyDegreesClockwise();
        if(equals(other)) return true; //Test 90 degree mirror state
        other.turnNinetyDegreesClockwise();
        if(equals(other)) return true; //Test 180 degree mirror state
        other.turnNinetyDegreesClockwise();
        if(equals(other)) return true; //Test 270 degree mirror state
    }
    return false;
}


void RoombotBuildPlan::print() const
{
    std::cout << "Organism build plan: " << std::endl;
    for(size_t i=0; i<positions.size(); i++){
        std::cout << "x: " << positions[i].x << " z: " << positions[i].z << " horizontal: " << positions[i].isHorizontal << std::endl;
    }
}


Bitmap* RoombotBuildPlan::paint() const
{
    Bitmap* bitmap = new Bitmap(POINT_SIZE + gridSize*(POINT_SIZE*2+1), POINT_SIZE + gridSize*(POINT_SIZE*2+1));
    for(size_t j=0; j<positions.size(); j++){
        RelativePosition position = positions.at(j);
        int x1 = POINT_SIZE + position.getX()*(POINT_SIZE*2+1);
        int x2 = POINT_SIZE + position.getX2()*(POINT_SIZE*2+1);
        int y1 = POINT_SIZE + position.getZ()*(POINT_SIZE*2+1);
        int y2 = POINT_SIZE + position.getZ2()*(POINT_SIZE*2+1);
        bitmap->drawPoint(x1, y1, POINT_SIZE);
        bitmap->drawPoint(x2, y2, POINT_SIZE);
        bitmap->drawBox(x1-POINT_SIZE, y1-POINT_SIZE, x2+POINT_SIZE, y2+POINT_SIZE, 0, 100, 100, 100);
    }
    
    return bitmap;
}

RelativePosition RoombotBuildPlan::getMinimalPosition() const
{
    if(positions.size() == 0) throw CREATE_LOCATEDEXCEPTION_INFO ("Can not get minimal position in empty buildplan");
    RelativePosition result = positions.at(0);
    
    for(size_t i=1; i<positions.size(); i++){
        if(positions.at(i) < result){
            result = positions.at(i);
        }
    }
    
    return result;
}


bool RoombotBuildPlan::occupied(int x, int z) const
{
    for(size_t i=0; i<positions.size(); i++){
        if(positions[i].getX() == x && positions[i].getZ() == z) return true;
        if(positions[i].getX2() == x && positions[i].getZ2() == z) return true;
    }
    
    return false;
}


size_t RoombotBuildPlan::find(const RelativePosition& position, size_t start) const
{
    for(size_t i=start; i<positions.size(); i++){
        if(positions.at(i).equals(position)) return i;
    }
    return positions.size();
}


bool RoombotBuildPlan::equals(RoombotBuildPlan other, bool ignoreShift) const
{
    if(positions.size() != other.size()) return false;
    if(positions.size() == 0) return true;
    
    if(ignoreShift){
        RelativePosition myBasePosition = getMinimalPosition();
        RelativePosition otherBasePosition = other.getMinimalPosition();
        if(myBasePosition.isHorizontal != otherBasePosition.isHorizontal) return false;
        other.shift(myBasePosition.x - otherBasePosition.x, myBasePosition.z - otherBasePosition.z);
    }
    
    bool array[positions.size()];
    for(size_t i=0; i<positions.size(); i++ ) array[i] = false;
    size_t index;
    
    for(size_t i=0; i<positions.size(); i++){
        index = other.find(positions.at(i));
        while(index < positions.size()){
            //This location was already matched, look for an other matching location
            if(array[index]){
                index = other.find(positions.at(i), index+1);
            } else {
                //Module found, mark this index for buildplans with redundant modules
                array[index] = true;
                break;
            }
        }
        //No matching index was found, these buildplans are not the same.
        if(index >= positions.size()) return false;
    }
    return true;
}


void RoombotBuildPlan::shift(int x, int z)
{
    for(size_t i=0; i<positions.size(); i++){
        positions.at(i).x += x;
        positions.at(i).z += z;
    }
}


void RoombotBuildPlan::flipHorizontally()
{
    for(size_t i=0; i<positions.size(); i++){
        positions.at(i).x = positions.at(i).x * -1;
        if(!positions.at(i).isHorizontal){
            positions.at(i).x += 1;
        }
    }
}


void RoombotBuildPlan::flipVertically()
{
    for(size_t i=0; i<positions.size(); i++){
        positions.at(i).z = positions.at(i).z * -1;
        if(positions.at(i).isHorizontal){
            positions.at(i).z += 1;
        }
    }
}


void RoombotBuildPlan::turnAndMirror()
{
    int temp;
    for(size_t i=0; i<positions.size(); i++){
        temp = positions.at(i).x;
        positions.at(i).x = positions.at(i).z;
        positions.at(i).z = temp;
        positions.at(i).isHorizontal = !positions.at(i).isHorizontal;
    }
}


void RoombotBuildPlan::turnNinetyDegreesClockwise()
{
    turnAndMirror();
    flipVertically();
}


void RoombotBuildPlan::turnNinetyDegreesCounterClockwise()
{
    turnAndMirror();
    flipHorizontally();
}