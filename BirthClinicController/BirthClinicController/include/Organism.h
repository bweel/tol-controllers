#ifndef ORGANISM_H_
#define ORGANISM_H_

#include "Module.h"
#include "BuildPlan.h"
#include "Position.h"
#include "ParametersReader.h"

#include <vector>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <fstream>

using boost::property_tree::ptree;
using boost::property_tree::write_json;


/**
 * The Organism class is used to store all information about an organism created by the Organism Manager,
 * such as its time-to-live, its genome and which modules it consists of.
 * An Organism object is created every time an egg successfully hatches,
 * and it is then stored by the organism manager until its time-to-hatch expires.
 */
class Organism
{
    
    int INFANCY_DURATION = ParametersReader::get<int>("INFANCY_DURATION");
    int ROOMBOT_WAITING_TIME = ParametersReader::get<int>("ROOMBOT_WAITING_TIME");
    int MATURE_TIME_TO_LIVE = ParametersReader::get<int>("MATURE_TIME_TO_LIVE");
    
    
	
    id_t id;                            //Unique id for this instance of organism.
	std::vector<Module*> robots; 		//An array of all robots part of this organism.
    std::string genome;                 //The genome of this organism.
	Position organismCentre;            //The position used as the basis during organism construction.
	std::auto_ptr<BuildPlan> buildPlan;	//The build plan used to create this organism.
    

    
    public:
    
	/**
	 * Constructs the Organism.
	 * Requires a genome, id, build-plan and position for the organism to be constructed.
	 *
	 * @param genome The id of the genome for this organism.
	 * @param id The id of the organism.
	 * @param buildPlan The build plan for the organism.
	 * @param organismCentre The location used as a `centre' used in combination
	 * with the build plan to determine where the organism will be build.
	 */
	Organism(std::string genome, id_t id, std::auto_ptr<BuildPlan> plan, Position organismCentre);
    
	/**
	 * Destructs the organism.
	 * Does not actually perform any additional clean-up.
	 */
	virtual ~Organism();
    
	/**
	 * Sets the genome id for this organism.
	 *
	 * @param _genome The new genome id for this organism.
	 */
	void setGenome(std::string g);
    
	/**
	 * Returns the genome id of this organism.
	 *
	 * @return Returns the genome id of this organism.
	 */
    std::string getGenome();
    
	/**
	 * Returns the id of this organism
	 *
	 * @return Returns the id of this organism
	 */
	id_t getId();
    
    std::string getName();
    
	/**
	 * Returns the size of this organism.
	 *
	 * @return Returns the size of this organism
	 */
	size_t getSize();
    
	/**
	 * Adds a module to this organism.
	 * The number of modules should be exactly the number of modules needed to create the build-plan
	 * associated with this organism.
	 *
	 * @param robot Pointer to a module.
	 */
	void addModule(Module * module);
    
    Module * getModule(int i);
    
	/**
	 * Puts all modules added to this organism at a position indicated by the build plan,
	 * effectively building the organism. Can be called multiple times to keep the organisms
	 * modules `in place' while connectors are locked. Does not send the ASSEMBLE and ACTIVATE commands
	 * necessary for building the organism. This should be done separately.
	 */
	void build();
    
    void activate();
    
    void writeControllerArgsFile(std::string simulationDateAndTime);
    
	/**
	 * Returns a string with the positions of every module in the organism.
	 * Used for logging and debugging only.
	 *
	 * @return Returns a string containing the positions of every module in the organism.
	 */
	std::string positionsToString();
    
    std::string createPathForControllerArgs();
};

#endif /* ORGANISM_H_ */