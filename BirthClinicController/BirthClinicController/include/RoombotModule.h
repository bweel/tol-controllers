#ifndef OM_ROOMBOT_MODULE_H_
#define OM_ROOMBOT_MODULE_H_

#include "Module.h"

#include <webots/Robot.hpp>
#include <webots/Node.hpp>
#include "NEAT.h"



using namespace webots;

/**
 * The RoombotModule class is used as a handle for Roombot modules.
 * Since it extends the pure virtual Module class this class only contains
 * the Roombot specific functions.
 * In general it is used to set the position of modules,
 * send messages to the modules and store information for each module.
 */
class RoombotModule : public Module
{
	Node * root;			//The root node of this module
	Field * childrenRoot;	//The children of the root node of this module
	Field * _translation;	//The position of this module
	Field * rotation;		//The rotation of this module
	Field * name;			//The 'in webots' name, not to be mistaken by the DEF name
    Field * controllerArgs;
    Field * controller;
    
	Node * receiver;		//The receiver of this module
	Field * channel;		//The channel of this module, should be equal to its id
    
    Field * CB1XLocked;
    Field * CB1YLocked;
    Field * CB1ZLocked;
    Field * CW1XLocked;
    Field * CW1ZLocked;
    Field * CB2XLocked;
    Field * CB2ZLocked;
    Field * CW2XLocked;
    Field * CW2YLocked;
    Field * CW2ZLocked;
    
    
    public:
    
	/**
	 * Constructs a Roombot module handle.
	 * First calls the Module constructor to set the Module variables.
	 * Will initialize all necessary nodes to get the position, rotation and receiver channel.
	 *
	 * @param root The root node, found by calling the getFromDef() function in the supervisor.
	 * @param id The unique id for this module.
	 * @param supervisorEmitter The emitter this object will use to send message to the module.
	 * @param arenaSize The size of the arena as the length of a single side in Webot meters.
	 * Used to place modules outside of the arena.
	 */
	RoombotModule(Node * root = NULL, id_t id = 0, double arenaSize = 0);
    
	/**
	 * Destructs the RoomModule.
	 * Does not actually perform any additional clean-up.
	 */
	virtual ~RoombotModule();
    
	/**
	 * Returns the name of this module.
	 * The name of the Roombot module is the value of the Webots field 'name'.
	 *
	 * @return Returns the name of this module.
	 */
	std::string getName() const;
    
    std::string getDef() const;
    
	/**
	 * Sets the translation and rotation of this module.
	 *
	 * @param position The position containing the new translation and rotation for this module.
	 */
	void setPosition(Position position);
    
    void setLock(std::string connectorName, bool lock);
    
    void setName(std::string organismID, std::string moduleID);
    
    void setController(std::string controllerName);
    
    void setControllerArgs(std::string path);
    
	/**
	 * Returns the current position of this module.
	 * The position object contains both the translation and rotation of the module.
	 *
	 * @return Returns the current position of this module.
	 */
	Position getPosition() const;
    
    
    private:
    
	/**
	 * Searches the multi-field for a node with the specified name and returns it.
	 *
	 * @param field The mutli-field to search in.
	 * @param name The name to search for.
	 * @return Returns the first node with name.
	 * @throws Throws an exception when the input field is not a multi-field
	 * or when the name is not present in the multi-field.
	 */
	Node * getNode(Field * field, std::string name) const;
};

#endif /* OM_ROOMBOT_MODULE_H_ */