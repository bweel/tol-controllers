#include "Position.h"


Position::Position(double x, double y, double z, bool isHorizontal)
{
    if(isHorizontal){
        WB_Rotation newRotation = HORIZONTAL_ROTATION;
        std::copy(newRotation, newRotation + ROTATION_LENGTH, rotation);
    } else {
        WB_Rotation newRotation = VERTICAL_ROTATION;
        std::copy(newRotation, newRotation + ROTATION_LENGTH, rotation);
    }
    
    translation[X_AXIS]=x;
    translation[Y_AXIS]=y;
    translation[Z_AXIS]=z;
}


Position::Position(const Translation _translation, const WB_Rotation _rotation){
    std::copy(_translation, _translation + TRANSLATION_LENGTH, translation);
    std::copy(_rotation, _rotation + ROTATION_LENGTH, rotation);
}


double Position::get(const int &index) const{
    return translation[index];
}


void Position::setX(double x){
    translation[X_AXIS]=x;
}


double Position::getX() const{
    return translation[X_AXIS];
}


void Position::setY(double y){
    translation[Y_AXIS]=y;
}


double Position::getY() const{
    return translation[Y_AXIS];
}


void Position::setZ(double z){
    translation[Z_AXIS]=z;
}


double Position::getZ() const{
    return translation[Z_AXIS];
}


double * Position::getTransaltion() {
    return translation;
}


double * Position::getRotation() {
    return rotation;
}


bool Position::checkBounds(double arenaSize) const{
    return
    translation[X_AXIS]<= (arenaSize/2) &&
    translation[X_AXIS]>= -(arenaSize/2) &&
    translation[Z_AXIS]<= (arenaSize/2) &&
    translation[Z_AXIS]>= -(arenaSize/2);
}


double Position::distance(const Position &other) const{
    double result=0;
    int i;
    
    for(i=0; i<TRANSLATION_LENGTH; i++){
        result += std::pow(translation[i]-other.get(i), 2.0);
    }
    
    return std::sqrt(result);
}


std::string Position::toString() const{
    std::string result = std::string(TO_STR(translation[X_AXIS]));
    result.append(" ");
    result.append(TO_STR(translation[Y_AXIS]));
    result.append(" ");
    result.append(TO_STR(translation[Z_AXIS]));
    result.append(" ");
    
    return result;
}


bool Position::operator<(const Position &other) const {
    if(getX()<other.getX()) return true;
    if(getX()>other.getX()) return false;
    if(getZ()<other.getZ()) return true;
    if(getZ()>other.getZ()) return false;
    if(getY()<other.getY()) return true;
    if(getY()>other.getY()) return false;
    return false;
}


void Position::print() const{
    std::cout << "x: " << translation[X_AXIS] << " y: " << translation[Y_AXIS] << "z: " << translation[Z_AXIS] << std::endl;
}