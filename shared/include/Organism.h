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
 */
class Organism
{
public:
    enum State {INFANT, ADULT, DEAD};
    
protected:
    
    int INFANCY_DURATION = ParametersReader::get<int>("INFANCY_DURATION");
    int ROOMBOT_WAITING_TIME = ParametersReader::get<int>("ROOMBOT_WAITING_TIME");
    int TIME_TO_LIVE = ParametersReader::get<int>("TIME_TO_LIVE");
    int EVALUATIONS = ParametersReader::get<int>("EVALUATIONS");
	
    id_t id;                            //Unique id for this instance of organism.
    State state;
    bool fertile;
    // Used by the evolver
    double fitness;
    unsigned int size;
    unsigned int offspring;
    std::vector<id_t> parents;
    std::string genome;                 //The genome of this organism.
    std::string mindGenome;             //The genome of this organims' mind.
    
public:

    Organism(const Organism& other);
    
    Organism(std::string genome, std::string mindGenome, id_t organismID, double fitness, unsigned int size, unsigned int offspring, std::vector<id_t> parents, State state, bool fertile);
    
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
	 * Sets the genome id for this organism.
	 *
	 * @param _mind The new mind for this organism.
	 */
	void setMind(std::string m);
    
	/**
	 * Returns the genome id of this organism.
	 *
	 * @return Returns the mind of this organism.
	 */
    std::string getMind();
    
    void setFitness(double fitness);
    
    double getFitness();
    
    void setState(State state);
    
    State getState();
    
    void setFertile(bool fertile);
    
    bool getFertile();
    
    void setSize(unsigned int size);
    
    unsigned int getSize();
    
    void setOffspring(unsigned int offspring);
    
    unsigned int getOffspring();
    
    void setParents(std::vector<id_t> parents);
    
    std::vector<id_t> getParents();
    
	/**
	 * Returns the id of this organism
	 *
	 * @return Returns the id of this organism
	 */
	id_t getId();
    
    std::string getName();
};
    
class BuildableOrganism : public Organism {
private:
    // Used by the birthclinic
    std::vector<Module*> robots; 		//An array of all robots part of this organism.
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
	BuildableOrganism(std::string genome, std::string mindGenome, id_t id, std::auto_ptr<BuildPlan> plan, Position organismCentre);
    
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
    
    void activate(std::string simulationDateAndTime);
    
    void writeControllerArgsFile(std::string simulationDateAndTime);
    
	/**
	 * Returns a string with the positions of every module in the organism.
	 * Used for logging and debugging only.
	 *
	 * @return Returns a string containing the positions of every module in the organism.
	 */
	std::string positionsToString();
    
    std::string createPathForControllerArgs(std::string simulationDateAndTime);
};

#endif /* ORGANISM_H_ */