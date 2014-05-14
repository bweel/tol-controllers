#include "Organism.h"


Organism::Organism(std::string genome, id_t id, std::auto_ptr<BuildPlan> plan, Position organismCentre) :
id(id), robots(), genome(genome), organismCentre(organismCentre), buildPlan(plan)
{
    //nix
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


id_t Organism::getId()
{
    return id;
}

std::string Organism::getName()
{
    return ORGANISM_BASE_NAME + std::to_string(id);
}


size_t Organism::getSize()
{
    return buildPlan->size();
}


void Organism::addModule(Module * module)
{
    robots.push_back(module);
}


Module * Organism::getModule(int i)
{
    return robots[i];
}



//**************************
//**** Other functions *****
//**************************

std::string Organism::positionsToString()
{
    std::string result = std::string();
    
    for (size_t i = 0; i < robots.size(); i++) {
        result.append(robots[i]->positionToString() + " ");
    }
    
    return (result);
}

std::string Organism::createPathForControllerArgs()
{
    return CONTROLLER_ARGS_PATH + getName() + ".json";
}

//**************************
//***** Main functions *****
//**************************


void Organism::build()
{
    for(size_t i = 0; i < buildPlan->size(); i++){
        if (robots[i] != 0)
        {
            // set moduele position
            Position position = buildPlan->getPosition(organismCentre, i);
            robots[i]->setPosition(position);
            std::cout << robots[i]->getName() << " placed in position "; position.print();
            
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


void Organism::writeControllerArgsFile(std::string simulationDateAndTime)
{
    std::cout << "writing ControllerArgs file for " << getName() << std::endl;
    
    // extract stuff to write from organism
    std::string organismId = std::to_string(id);
    std::string size = std::to_string(getSize());
    
    // open file
    std::string filePath = createPathForControllerArgs();
    ofstream organismFile(filePath);
    
    // build tree
    ptree pt;
    pt.put("ID", "organism_" + organismId);
    pt.put("Simulation", simulationDateAndTime);
    // Algorithm
    ptree child;
    child.put("Type", "2");
    child.put("Parameters", "");
    child.put("Save", "");
    child.put("Infancy_Duration",INFANCY_DURATION);
    child.put("Evaluations", "50");
    child.put("Angular_Velocity", "2.6");
    pt.put_child("Algorithm", child);
    // Shape
    child.clear();
    child.put("X", "3");
    child.put("Y", "1");
    child.put("Z", "5");
    pt.put_child("Shape", child);
    // Robot
    child.clear();
    child.put("Index", "0");
    child.put("Index_Root", "0");
    child.put("Name", getName());
    child.put("Modules_#", size);
    child.put("Controller", ROOMBOT_CONTROLLER_NAME);
    child.put("Arguments", filePath);
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
    
    // write and close
    write_json(organismFile, pt, false);
    organismFile.close();
}


void Organism::activate()
{
    for(size_t i = 0; i < buildPlan->size(); i++){
        if (robots[i] != 0)
        {
            // update controllerArgs in Webots
            robots[i]->setControllerArgs(createPathForControllerArgs());
            std::cout << robots[i]->getDef() << " -> controllerArgs has been updated" << std::endl;
            
            // update controller in Webots
            robots[i]->setController(ROOMBOT_CONTROLLER_NAME);
            std::cout << robots[i]->getDef() << " -> " << ROOMBOT_CONTROLLER_NAME << " has been set" << std::endl;
        }
    }
}