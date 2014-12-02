#include "Organism.h"
#include "Random.h"

/**
 * Copy constructor that does not actually does a deep copy
 *
 */
Organism::Organism(const Organism &other) {
    id = other.id;
    genome = other.genome;
    mindGenome = other.mindGenome;
    fitness = other.fitness;
    parents = other.parents;
    offspring = other.offspring;
    size = other.size;
    state = other.state;
    fertile = other.fertile;
}

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
BuildableOrganism::BuildableOrganism(std::string genome, std::string mindGenome, id_t id, std::auto_ptr<BuildPlan> plan, Position organismCentre) :
Organism(genome,mindGenome,id,-1,0,0,std::vector<id_t>(),INFANT, false),
robots(), organismCentre(organismCentre), buildPlan(plan)
{
}

Organism::Organism(std::string genome, std::string mindGenome, id_t organismID, double fitness, unsigned int size, unsigned int offspring, std::vector<id_t> parents, State state, bool fertile) :
    fitness(fitness), id(organismID), genome(genome), mindGenome(mindGenome), size(size), offspring(offspring), parents(parents), state(state), fertile(fertile)
{
}

Organism::~Organism()
{
    //nix
}

//********************************
//**** Getters and Setters *******
//********************************


void Organism::setGenome(std::string g)
{
    genome = g;
}


std::string Organism::getGenome()
{
    return genome;
}

void Organism::setMind(std::string m)
{
    mindGenome = m;
}


std::string Organism::getMind()
{
    return mindGenome;
}

void Organism::setFitness(double f) {
    fitness = f;
}

double Organism::getFitness() {
    return fitness;
}


id_t Organism::getId()
{
    return id;
}

std::string Organism::getName()
{
    return ORGANISM_BASE_NAME + std::to_string(id);
}

void Organism::setState(State s) {
    state = s;
}

Organism::State Organism::getState() {
    return state;
}

void Organism::setFertile(bool f) {
    fertile = f;
}

bool Organism::getFertile() {
    return fertile;
}

void Organism::setSize(unsigned int s) {
    size = s;
}

unsigned int Organism::getSize() {
    return size;
}

void Organism::setOffspring(unsigned int o) {
    offspring = o;
}

unsigned int Organism::getOffspring() {
    return offspring;
}

void Organism::setParents(std::vector<id_t> p) {
    parents = p;
}

std::vector<id_t> Organism::getParents() {
    return parents;
}

size_t BuildableOrganism::getSize()
{
    return buildPlan->size();
}


void BuildableOrganism::addModule(Module * module)
{
    robots.push_back(module);
}


Module * BuildableOrganism::getModule(int i)
{
    return robots[i];
}

//**************************
//**** Other functions *****
//**************************

std::string BuildableOrganism::positionsToString()
{
    std::string result = std::string();
    
    for (size_t i = 0; i < robots.size(); i++) {
        result.append(robots[i]->positionToString() + " ");
    }
    
    return (result);
}

std::string BuildableOrganism::createPathForControllerArgs(std::string simulationDateAndTime)
{
    return CONTROLLER_ARGS_PATH + simulationDateAndTime + "/" + getName() + ".json";
}

//**************************
//***** Main functions *****
//**************************


void BuildableOrganism::build()
{
    for(size_t i = 0; i < buildPlan->size(); i++){
        if (robots[i] != 0)
        {
            // set module position
            Position position = buildPlan->getPosition(organismCentre, i);
            robots[i]->setPosition(position);
            
            // lock connectors
            std::cout << robots[i]->getName() << ": ";
            for (int c = 0; c < 10; c++)
            {
                bool locked = buildPlan->getConnector(i, c);
                robots[i]->setLock(CONNECTOR_NAMES[c],locked);
                if(locked) std::cout << CONNECTOR_NAMES[c] << " ";
            }
            std::cout << std::endl;
        }
    }
}


void BuildableOrganism::writeControllerArgsFile(std::string simulationDateAndTime)
{
    std::cout << "writing ControllerArgs file for " << getName() << std::endl;
    
    // extract stuff to write from organism
    std::string organismId = std::to_string(id);
    std::string size = std::to_string(getSize());
    
    // open file
    std::string filePath = createPathForControllerArgs(simulationDateAndTime);
    ofstream organismFile(filePath);
    
    // build tree
    ptree pt;
    pt.put("ID", "organism_" + organismId);
    pt.put("Simulation", simulationDateAndTime);
    // Algorithm
    ptree child;
    child.put("Type", "RLPower");
    child.put("Parameters", "");
    child.put("Save", "");
    child.put("Infancy_Duration",INFANCY_DURATION);
    child.put("Waiting_Time", ROOMBOT_WAITING_TIME);
    
    int noise = 0;
    if(TIME_TO_LIVE_NOISE > 0) {
        noise = Utils::Random::getInstance()->uniform_integer(-TIME_TO_LIVE_NOISE,TIME_TO_LIVE_NOISE);
    }
    child.put("Time_To_Live", TIME_TO_LIVE + noise);

    child.put("Evaluations", EVALUATIONS);
    child.put("Angular_Velocity", "2.6");
    pt.put_child("Algorithm", child);
//    // Shape              Not used
//    child.clear();
//    child.put("X", "3");
//    child.put("Y", "1");
//    child.put("Z", "5");
//    pt.put_child("Shape", child);
    // Robot
    child.clear();
    child.put("Name", getName());
    child.put("Modules_#", size);
//    child.put("Controller", ROOMBOT_CONTROLLER_NAME); // not used
//    child.put("Arguments", filePath);
        // Motor
        ptree child2;
        child2.put("Number", "3");
        child2.put("Range", "3.1415");
        child2.put("0", "s1");
        child2.put("1", "m1");
        child2.put("2", "s2");
        child.put_child("Motor", child2);
    for(int i = 0; i < getSize(); i++)
    {
        // module_x:x
        child2.clear();
        child2.put("Index", std::to_string(i));
        child2.put("Name", MODULE_BASE_NAME + organismId);
        if (i==0)
            child2.put("Type", "2");
        else
            child2.put("Type", "1");
        ptree child3;
        for(int c = 0; c < 10; c++)
        {
            // connectors
            ptree connector;
            if (buildPlan->getConnector(i, c))
            {
                //cout << CONNECTOR_NAMES[c] << " locked" << endl;
                connector.put("", CONNECTOR_NAMES[c]);
                child3.push_back(std::make_pair("", connector));
            }
        }
        //cout << child3.size() << endl;
        child2.put_child("Connectors", child3);
            // Index_3D
            child3.clear();
            child3.put("X", "0");
            child3.put("Y", "0");
            child3.put("Z", "0");
        child2.put_child("Index_3D", child3);
    child.put_child(robots[i]->getName(), child2);
        
    }
    pt.put_child("Robot", child);
    pt.put("Genome", genome);
    pt.put("MindGenome", mindGenome);
    
    // write and close
    write_json(organismFile, pt, true);
    organismFile.close();
}


void BuildableOrganism::activate(std::string simulationDateAndTime)
{
   for(size_t i = 0; i < buildPlan->size(); i++){
        if (robots[i] != 0)
        {
            // update controllerArgs in Webots
            robots[i]->setControllerArgs(createPathForControllerArgs(simulationDateAndTime));
            std::cout << robots[i]->getDef() << " -> controllerArgs has been updated" << std::endl;
            
            // update controller in Webots
            robots[i]->setController(LIFE_CONTROLLER_NAME);
            std::cout << robots[i]->getDef() << " -> " << LIFE_CONTROLLER_NAME << " has been set" << std::endl;
        }
    }
}
