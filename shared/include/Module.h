#ifndef OM_MODULE_H_
#define OM_MODULE_H_

#include "Position.h"

/**
 * The Module class is a pure virtual class used as a handle for the Organism Manager.
 * Whenever the Organism Manager wants to send a message to a module,
 * move a module, or attach some information to a module it uses the
 * Module object corresponding to the actual module.
 *
 * Since different types of module might have position,
 * receiver and name information at different places in the Webots tree
 * These functions have to be implemented separately for each type of module.
 */
class Module
{
    
    protected:
    
	id_t id;            //The id of the module.
	bool assembled;     //Indicates whether this module is assembled
	double arenaSize;   //Stores the arena size for putting the module outside the arena.
	bool inReserve;		//Indicates whether this module is in reserve.
    
    
    public:
	
     /**
	 * Initializes all variables that are part of the Module class.
	 * Since Module is a pure virtual class, this constructor can not
	 * be used to create a Module object, and it should only be used
	 * by classes that extend Module.
	 *
	 * @param id The unique id for this module.
	 * @param arenaSize The size of the arena used to place modules outside the arena.
	 */
	Module(id_t id = 0, double arenaSize = 0);
    
	/**
	 * Sets the assembled parameter.
	 * The assembled parameter indicates whether this module is assembled,
	 * meaning that all necessary connectors are connected.
	 *
	 * @param value The new value for the assembled parameter.
	 */
	//void setAssembled(bool value);
    
	/**
	 * Returns the value of the assembled parameter.
	 * The assembled parameter indicates whether this module is assembled,
	 * meaning that all necessary connectors are connected.
	 *
	 * @return Returns the value of the assembled parameter.
	 */
	//bool getAssembled() const;
    
	/**
	 * Returns the id of this module.
	 * The module id should not be confused with the egg id or organism id.
	 *
	 * @return Returns the id of this module
	 */
	id_t getId() const;
    
	/**
	 * Returns whether this module is in reserve .
	 * Modules that are in reserve might still need some time to reset before
	 * they can be used in the construction of new organisms.
	 *
	 * @return Returns true if this module is in reserve. Returns false otherwise.
	 */
	bool getInReserve() const;
    
	/**
	 * Moves this module to its reserve location outside the arena and sets the module to `in reserve'.
	 * Moving the module to reserve does not cause the module to deactivate,
	 * this has to be done separately by sending a deactivate command to the module.
	 */
    void toReserve();
    
	/**
	 * Sets this module to not be `in reserve'.
	 */
	void removeFromReserve();
    
	/**
	 * Returns the position of this module as a string.
	 * Used for logging and debugging.
	 *
	 * @return Returns the position of this module as a string.
	 */
	std::string positionToString() const;
    
	/**
	 * Function that should return the name of this module.
	 * The name of the module is usually the value of the Webots `name' field.
	 *
 	 * @return Returns the name of this module.
	 */
	virtual std::string getName() const = 0;
    
    virtual std::string getDef() const = 0;
    
	/**
	 * Function that should set the position of the module.
	 * Changing the position of the module will cause the module to teleport in the simulation.
	 *
	 * @param position The new position for the module.
	 */
	virtual void setPosition(Position position) = 0;
    
	virtual void setLock(std::string connectorName, bool lock) = 0;
    
    virtual void setName(std::string organismID, std::string moduleID) = 0;
    
    virtual void setController(std::string controllerName) = 0;
    
    virtual void setControllerArgs(std::string path) = 0;
    
	/**
	 * Function that should return the position, including angular position, of this module.
	 *
	 * @return Returns the position of this module.
	 */
	virtual Position getPosition() const = 0;
    
	/**
	 * Function that should send a message to this module.
	 * The right channel is usually found by accessing the receiver of the module,
	 * and then sending the message to that channel.
	 * Only works if every module has a unique channel.
	 *
	 * @param message The message to send to this module.
	 */
	//virtual void sendMessage(om_message message) const = 0;
    
};

#endif /* OM_MODULE_H_ */