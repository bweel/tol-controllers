#include "RoombotModule.h"


RoombotModule::RoombotModule(Node * root, id_t id, double arenaSize) : Module(id, arenaSize), root(root)
{
    if(!root) return;
    
    childrenRoot = root->getField("children");
    _translation = root->getField("translation");
    rotation = root->getField("rotation");
    name = root->getField("name");
    controller = root->getField("controller");
    controllerArgs = root->getField("controllerArgs");
    
    CB1XLocked = root->getField("CB1XLocked");
    CB1YLocked = root->getField("CB1YLocked");
    CB1ZLocked = root->getField("CB1ZLocked");
    CW1XLocked = root->getField("CW1XLocked");
    CW1ZLocked = root->getField("CW1ZLocked");
    CB2XLocked = root->getField("CB2XLocked");
    CB2ZLocked = root->getField("CB2ZLocked");
    CW2XLocked = root->getField("CW2XLocked");
    CW2YLocked = root->getField("CW2YLocked");
    CW2ZLocked = root->getField("CW2ZLocked");
    
    CB1XLocked->setSFBool(false);
    CB1YLocked->setSFBool(false);
    CB1ZLocked->setSFBool(false);
    CW1XLocked->setSFBool(false);
    CW1ZLocked->setSFBool(false);
    CB2XLocked->setSFBool(false);
    CB2ZLocked->setSFBool(false);
    CW2XLocked->setSFBool(false);
    CW2YLocked->setSFBool(false);
    CW2ZLocked->setSFBool(false);
    
    // Added an if-condition to check if childrenRoot > 0. Seems normal modules don't have a
    // children field. @Daniel
    if (childrenRoot) {
        receiver = getNode(childrenRoot, "supervisor_receiver");
        channel = receiver->getField("channel");
    } else {
        channel = root->getField("supervisor_receiver_channel");
    }
}


RoombotModule::~RoombotModule()
{
    //nix
}


//********************************
//***** Getters and Setters ******
//********************************

std::string RoombotModule::getName() const
{
    return name->getSFString();
}

std::string RoombotModule::getDef() const
{
    return MODULE_DEF_BASE_NAME + std::to_string(id);
}


void RoombotModule::setPosition(Position position)
{
    removeFromReserve();
    _translation->setSFVec3f(position.getTransaltion());
    rotation->setSFRotation(position.getRotation());
}


void RoombotModule::setLock(std::string connectorName, bool lock)
{
    if (connectorName.compare("CB1X") == 0) { CB1XLocked->setSFBool(lock); return;}
    if (connectorName.compare("CB1Y") == 0) { CB1YLocked->setSFBool(lock); return;}
    if (connectorName.compare("CB1Z") == 0) { CB1ZLocked->setSFBool(lock); return;}
    if (connectorName.compare("CW1X") == 0) { CW1XLocked->setSFBool(lock); return;}
    if (connectorName.compare("CW1Z") == 0) { CW1ZLocked->setSFBool(lock); return;}
    if (connectorName.compare("CB2X") == 0) { CB2XLocked->setSFBool(lock); return;}
    if (connectorName.compare("CB2Z") == 0) { CB2ZLocked->setSFBool(lock); return;}
    if (connectorName.compare("CW2X") == 0) { CW2XLocked->setSFBool(lock); return;}
    if (connectorName.compare("CW2Y") == 0) { CW2YLocked->setSFBool(lock); return;}
    if (connectorName.compare("CW2Z") == 0) CW2ZLocked->setSFBool(lock);
}


void RoombotModule::setName(std::string organismID, std::string moduleID)
{
    std::string moduleName = MODULE_BASE_NAME + organismID + ":" + moduleID;
    name->setSFString(moduleName);
}


void RoombotModule::setController(std::string controllerName)
{
    controller->setSFString(controllerName);
}


void RoombotModule::setControllerArgs(std::string path)
{
    controllerArgs->setSFString(path);
}


Position RoombotModule::getPosition() const
{
    return Position(_translation->getSFVec3f(), rotation->getSFRotation());
}


Node * RoombotModule::getNode(Field * field, std::string name) const
{
    int fieldCount = field->getCount();
    Node * node;
    Field * nameField;
    
    if (fieldCount == -1)
        throw CREATE_LOCATEDEXCEPTION_INFO("Field is not a multi-field.");
    
    for (int i = 0; i < fieldCount; i++) {
        node = field->getMFNode(i);
        nameField = node->getField("name");
        
        if (nameField != NULL) {
            
            if (nameField->getSFString().compare(name) == 0) {
                return node;
            }
        }
    }
    
    throw CREATE_LOCATEDEXCEPTION_INFO("Field: " + name + " not found.");
}