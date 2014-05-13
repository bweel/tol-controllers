#include "BirthClinicController.h"


////////////////////////////////////////////
///////////// MINOR FUNCTIONS //////////////
////////////////////////////////////////////

id_t BirthClinicController::getNextOrganismId()
{
    nextOrganismId++;
    return nextOrganismId-1;
}


int BirthClinicController::getFreeSlide()
{
    int bestSlide = -1;
    int bestScore = 1000;
    for (int i = 0; i < NUMBER_OF_SLIDES; i++)
    {
        int score = countInRadius(slidesPosition[i], CLINIC_RAIUS);
        if (score < bestScore)
        {
            bestSlide = i;
            bestScore = score;
        }
    }
    return bestSlide;
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


bool BirthClinicController::buildOrganism(CppnGenome genome)
{
    std::auto_ptr<BuildPlan> buildPlan = builder->translateGenome(genome);
    
    if (buildPlan->size() > 1)
    {
        // add modules to organism
        if (availableModules.size() >= buildPlan->size())
        {
            // create an organism object and add the modules
            
            int freeslide = getFreeSlide();
            if (freeslide != -1)
            {
                size_t buildPlanSize = buildPlan->size();   // after creating the new organism the pointer to the build plan disappears
                Organism * organism = new Organism(genome.toString(), getNextOrganismId(), buildPlan, slidesPosition[freeslide]);
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
                organism->writeControllerArgsFile();
                organism->activate();
                return false;
            }
            else
            {
                std::cout << "ORGANISM NOT CREATED: waiting for clinic to be free" << std::endl;
                return true;
            }
        }
        else
        {
            std::cout << "ORGANISM NOT CREATED: not enough nodules available" << std::endl;
            return false;
        }
    }
    else
    {
        std::cout << "ORGANISM NOT CREATED: the build plan was empty or made of only one module" << std::endl;
        return false;
    }
}


std::string BirthClinicController::readUpdateAvailableMessage(std::string message)
{
    return message.substr(16, message.length());
}



////////////////////////////////////////////
////////////// MAIN FUNCTIONS //////////////
////////////////////////////////////////////

BirthClinicController::BirthClinicController() : Supervisor()
{
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(CLINIC_CHANNEL);
    emitter = getEmitter(EMITTER_NAME);
    
    // slides positions
    slidesPosition[0] = Position((ARENA_SIZE/2)-1, 1, (ARENA_SIZE/2)-1);
    slidesPosition[1] = Position((ARENA_SIZE/2)-1, 1, -((ARENA_SIZE/2)-1));
    slidesPosition[2] = Position(-((ARENA_SIZE/2)-1), 1, (ARENA_SIZE/2)-1);
    slidesPosition[3] = Position(-((ARENA_SIZE/2)-1), 1, -((ARENA_SIZE/2)-1));
    
    // setup shape encoding
    switch (SHAPE_ENCODING) {
        case CPPN:
            builder = new Builder();
            break;
            
        default:
            break;
    }
    
    // update id for next organism
    nextOrganismId = INITIAL_POPULATION + 1;
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
                std::istringstream stream(message);
                CppnGenome genome = builder->getGenomeFromStream(stream);
                bool waiting = buildOrganism(genome);
                if (!waiting)
                {
                    receiver->nextPacket();
                }
            }
        }
    }
}