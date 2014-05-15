#include "BirthClinicController.h"


////////////////////////////////////////////
///////////// MINOR FUNCTIONS //////////////
////////////////////////////////////////////

id_t BirthClinicController::getNextOrganismId()
{
    nextOrganismId++;
    return nextOrganismId-1;
}


int BirthClinicController::countInRadius(Position position, double radius)
{
    std::map<id_t, Module*>::iterator it;
    
    int counter = 0;
    
    for ( it=moduleMap.begin() ; it != moduleMap.end(); it++ )
    {
        if(position.distance(Position(it->second->getPosition())) < radius)
        {
            counter++;
        }
    }
    return counter;
}


bool BirthClinicController::positionFree(Position position, double size)
{
    std::map<id_t, Module*>::iterator it;
    
    if(!position.checkBounds(ARENA_SIZE)) return false;
    
    for ( it=moduleMap.begin() ; it != moduleMap.end(); it++ )
    {
        if(position.distance(Position(it->second->getPosition())) < size)
        {
            return false;
        }
    }
    return true;
}


Position BirthClinicController::getFreeRandomPosition(double size)
{
    Position result;
    result.setY(GROUND_HEIGHT);
    
    int maxCoordinate = (ARENA_SIZE / 2) - 1;
    
    int tryNumber = 0;
    while(tryNumber < 50){
        tryNumber++;
        result.setX(RANDOM.getRandomDouble(-maxCoordinate, maxCoordinate));
        result.setZ(RANDOM.getRandomDouble(-maxCoordinate, maxCoordinate));
        if (positionFree(result, size))
            return result;
    }
    result.setX(std::numeric_limits<double>::max());
    result.setZ(std::numeric_limits<double>::max());
    return result;
}


void BirthClinicController::sendStartMessage(int channel)
{
    emitter->setChannel(channel);
    std::string message = "START";
    emitter->send(message.c_str(), (int)message.length()+1);
}


void BirthClinicController::addModuleToReserve(std::string moduleDef)
{
    Node * root = getFromDef(moduleDef);
    if (root)
    {
        int idx = std::atoi(moduleDef.substr(moduleDef.find("_")+1, moduleDef.length()).c_str());
        
        std::cout << "module " << idx << " added to available" << std::endl;
        
        moduleMap[idx]->toReserve();
        availableModules.push(idx);
    }
    else
    {
        throw CREATE_LOCATEDEXCEPTION_INFO(moduleDef + " does not exist.");
    }
}


void BirthClinicController::readGenomeMessage(std::string message, std::string * genomeStr, id_t * parent1, id_t * parent2)
{
    *genomeStr = message.substr(message.find("GENOME")+6, message.find("PARENTS")-6);
    std::string parentsSubStr = message.substr(message.find("PARENTS")+7, message.length());
    *parent1 = std::atoi(parentsSubStr.substr(0, parentsSubStr.find("-")).c_str());
    *parent2 = std::atoi(parentsSubStr.substr(parentsSubStr.find("-")+1, parentsSubStr.length()).c_str());
}


std::string BirthClinicController::readUpdateAvailableMessage(std::string message)
{
    return message.substr(16, message.length());
}


void BirthClinicController::storePhilogenyOnFile(id_t parent1, id_t parent2, id_t newBorn)
{
    ofstream philogenyFile;
    philogenyFile.open(RESULTS_PATH + simulationDateAndTime + "/philogenetic_tree.txt", ios::app);
    philogenyFile << std::to_string(parent1) + " " + std::to_string(parent2) + " " + std::to_string(newBorn) + "\n";
    philogenyFile.close();
}

void BirthClinicController::rotate()
{
    Field * rotation = platform->getField("rotation");
    double angle = (rand() % (2*31459)) / 10000;
    double r[4] = {0, 1, 0, angle};
    rotation->setSFRotation(r);
}


////////////////////////////////////////////
////////////// INITIALIZATION //////////////
////////////////////////////////////////////

void BirthClinicController::connectModulesToObjects()
{
    // connect real modules to objects and store them into map and stack
    for (id_t i = 1; i < (NUMBER_OF_MODULES+1); i++)
    {
        Node * root = getFromDef(MODULE_DEF_BASE_NAME + TO_STR(i));
        if (root)
        {
            moduleMap[i] = new RoombotModule(root, i, ARENA_SIZE);
            moduleMap[i]->toReserve();
            availableModules.push(i);
            std::cout << MODULE_DEF_BASE_NAME + TO_STR(i) + " INITIALIZED" << std::endl;
        }
        else
        {
            throw CREATE_LOCATEDEXCEPTION_INFO(MODULE_DEF_BASE_NAME + TO_STR(i) + " does not exist.");
        }
    }
}


////////////////////////////////////////////
///////////////// BUILDING /////////////////
////////////////////////////////////////////


int BirthClinicController::buildOrganism(CppnGenome genome)
{
    std::auto_ptr<BuildPlan> buildPlan = builder->translateGenome(genome);
    
    if (buildPlan->size() > 1)
    {
        // add modules to organism
        if (availableModules.size() >= buildPlan->size())
        {
            // create an organism object and add the modules
            if (countInRadius(position, CLINIC_SAFE_DISTANCE) == 0)
            {
                rotate();
                
                size_t buildPlanSize = buildPlan->size();   // after creating the new organism the pointer to the build plan disappears
                Organism * organism = new Organism(genome.toString(), getNextOrganismId(), buildPlan, position);
                std::cout << "NEW ORGANISM CREATED: " << organism->getName() << std::endl;
                
                for(size_t i = 0; i < buildPlanSize; i++)
                {
                    // get module from map and remove from available stack
                    Module * moduleToAdd = moduleMap[availableModules.top()];
                    availableModules.pop();
                    
                    // update name in Webots
                    moduleToAdd->setName(std::to_string(organism->getId()), std::to_string(i));
                    std::cout << moduleToAdd->getDef() << " -> name has been updated to " << moduleToAdd->getName() << std::endl;
                    
                    // add it to the organism
                    organism->addModule(moduleToAdd);
                    std::cout << "module " << moduleToAdd->getId() << " added to "  << organism->getName() << std::endl;
                }
                // build organism, write relative ControllerArgs file and activate it (set controller and arguments fields on Webots)
                organism->build();
                organism->writeControllerArgsFile(simulationDateAndTime);
                organism->activate();
                return 1;
            }
            else
            {
                //std::cout << "ORGANISM NOT CREATED: waiting for clinic to be free" << std::endl;
                return 2;
            }
        }
        else
        {
            std::cout << "ORGANISM NOT CREATED: not enough nodules available" << std::endl;
            return 3;
        }
    }
    else
    {
        std::cout << "ORGANISM NOT CREATED: the build plan was empty or made of only one module" << std::endl;
        return 3;
    }
}



////////////////////////////////////////////
////////////// MAIN FUNCTIONS //////////////
////////////////////////////////////////////

BirthClinicController::BirthClinicController() : Supervisor()
{
    srand(static_cast<unsigned int>(time(NULL)));
    
    platform = getFromDef("CLINIC_PLATFORM");
    
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(CLINIC_CHANNEL);
    emitter = getEmitter(EMITTER_NAME);
    
    // setup shape encoding
    switch (SHAPE_ENCODING) {
        case CPPN:
            builder = new Builder();
            break;
            
        default:
            break;
    }
    
    // update id for next organism
    nextOrganismId = 1;
}


BirthClinicController::~BirthClinicController()
{
    
}


void BirthClinicController::run()
{
    double timeStep = getBasicTimeStep();
    
    receiver->enable(timeStep);
    while (receiver->getQueueLength() > 0)
    {
        receiver->nextPacket();
    }
    
    // wait unitil EnvironmentModifierController sais the environment is ok
    bool environmentOk = false;
    while (step(timeStep) != -1 && !environmentOk)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            if (message.substr(0,14).compare("ENVIRONMENT_OK") == 0)
            {
                environmentOk = true;
                simulationDateAndTime = message.substr(14,message.length());
            }
            receiver->nextPacket();
        }
    }
    
    connectModulesToObjects();
    
    while (step(timeStep) != -1)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            if(message.substr(0,16).compare("UPDATE_AVAILABLE") == 0)
            {
                std::string moduleDef = readUpdateAvailableMessage(message);
                addModuleToReserve(moduleDef);
                receiver->nextPacket();
            }
            else
            {
                std::string genomeStr;
                id_t parent1;
                id_t parent2;
                readGenomeMessage(message, & genomeStr, & parent1, & parent2);
                
                std::istringstream stream(message);
                CppnGenome genome = builder->getGenomeFromStream(stream);
                int buildResponse = buildOrganism(genome);
                
                if (buildResponse == 1)
                {
                    storePhilogenyOnFile(parent1, parent2, nextOrganismId-1);
                    receiver->nextPacket();
                }
                if (buildResponse == 3)
                {
                    receiver->nextPacket();
                }
                if (buildResponse == 2)
                {
                    
                }
            }
        }
    }
}