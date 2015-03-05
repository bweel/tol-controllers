#include <boost/lexical_cast.hpp>

#include "BirthClinicController.h"



//////////////////////////////////////////
/////////// MINOR FUNCTIONS //////////////
//////////////////////////////////////////

id_t BirthClinicController::getNextOrganismId()
{
    nextOrganismId++;
    return nextOrganismId-1;
}


bool BirthClinicController::isClinicFree()
{
    std::map<unsigned int, Module*>::iterator it;
    
    for ( it=moduleMap.begin() ; it != moduleMap.end(); it++ )
    {
        Module * module = it->second;
        
        if (module->getInReserve() == false)
        {
            double x = it->second->getPosition().getX();
            double z = it->second->getPosition().getZ();
            double distanceFromCenter = sqrt((x*x) + (z*z));
        
            if(distanceFromCenter < CLINIC_RADIUS)
            {
                return false;
            }
        }
    }
    return true;
}


int BirthClinicController::countInRadius(Position position, double radius)
{
    std::map<unsigned int, Module*>::iterator it;
    
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
    std::map<unsigned int, Module*>::iterator it;
    
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
        result.setX(random->uniform_real(-maxCoordinate, maxCoordinate));
        result.setZ(random->uniform_real(-maxCoordinate, maxCoordinate));
        if (positionFree(result, size))
            return result;
    }
    result.setX(std::numeric_limits<double>::max());
    result.setZ(std::numeric_limits<double>::max());
    return result;
}


void BirthClinicController::addModuleToReserve(std::string moduleDef)
{
    Node * root = getFromDef(moduleDef);
    if (root)
    {
        unsigned int idx = std::atoi(moduleDef.substr(moduleDef.find("_")+1, moduleDef.length()).c_str());
        
        moduleMap[idx]->toReserve();
        
        bool alreadyAvailable = false;
        std::stack<id_t> stackCopy = availableModules;
        while (!stackCopy.empty())
        {
            id_t i = stackCopy.top();
            stackCopy.pop();
            if (i == idx)
            {
                alreadyAvailable = true;
            }
        }
        if (!alreadyAvailable)
        {
            availableModules.push(idx);
            logger.debugStream() << "module " << idx << " added to available";
            logModuleCount("Module Added",availableModules.size());
        }
    }
    else
    {
        throw CREATE_LOCATEDEXCEPTION_INFO(moduleDef + " does not exist.");
    }
}


void BirthClinicController::readGenomeMessage(std::string message, std::string * genomeStr, std::string * mindStr, id_t * parent1, id_t * parent2, std::string * fitness1, std::string * fitness2)
{
    // Template:
    // GENOME<genome data>MIND<mind data>PARENTSparent1-parent2PARENTS_FITNESSfitness1-fitness2
    
    *genomeStr = MessagesManager::get(message, "GENOME");
    *mindStr = MessagesManager::get(message, "MIND");
    
    std::string parentsSubStr = MessagesManager::get(message, "PARENTS");
    *parent1 = std::atoi(parentsSubStr.substr(0, parentsSubStr.find("-")).c_str());
    *parent2 = std::atoi(parentsSubStr.substr(parentsSubStr.find("-")+1, parentsSubStr.length()).c_str());
    
    std::string fitnessSubStr = MessagesManager::get(message, "PARENTS_FITNESS");
    *fitness1 = fitnessSubStr.substr(0, fitnessSubStr.find("-")).c_str();
    *fitness2 = fitnessSubStr.substr(fitnessSubStr.find("-")+1, fitnessSubStr.length()).c_str();
}


void BirthClinicController::readRebuildMessage(std::string message, id_t * organismId, std::string * genomeStr, std::string * mindStr)
{
    // Template:
    // <organismId>GENOME<genome-data>MIND<mind-data>
    *organismId = std::atoi(MessagesManager::get(message, "ID").c_str());
    *genomeStr = MessagesManager::get(message, "GENOME");
    *mindStr = MessagesManager::get(message, "MIND");
}


void BirthClinicController::storePhilogenyOnFile(id_t parent1, id_t parent2, id_t newBorn, std::string fitness1, std::string fitness2)
{
    ofstream philogenyFile;
    philogenyFile.open(RESULTS_PATH + simulationDateAndTime + "/philogenetic_tree.txt", ios::app);
    philogenyFile << std::to_string(getTime()) + " " + std::to_string(newBorn) + " " + std::to_string(parent1) + " " + std::to_string(parent2) + " " + fitness1 + " " + fitness2 + "\n";
    philogenyFile.close();
}


void BirthClinicController::storeGenomeOnFile(id_t organismId, std::string genomeStr)
{
    ofstream genomesFile;
    genomesFile.open(RESULTS_PATH + simulationDateAndTime + "/genomes.txt", ios::app);
    genomesFile << "organism_" + std::to_string(organismId) + ": " + genomeStr + "\n";
    genomesFile.close();
}


void BirthClinicController::storeMindGenomeOnFile(id_t organismId, std::string mindGenomeStr)
{
    ofstream mindGenomesFile;
    mindGenomesFile.open(RESULTS_PATH + simulationDateAndTime + "/mind_genomes.txt", ios::app);
    mindGenomesFile << "organism_" + std::to_string(organismId) + ": " + mindGenomeStr + "\n";
    mindGenomesFile.close();
}


void BirthClinicController::rotate()
{
    Field * rotation = platform->getField("rotation");
    double angle = (rand() % (2*31459)) / 10000;
    double r[4] = {0, 1, 0, angle};
    rotation->setSFRotation(r);
}

void BirthClinicController::sendOrganismBuiltMessage(id_t parent1, id_t parent2, id_t organism, unsigned int size, std::string genome, std::string mind)
{
    std::string orgbuilt = "[ORGANISM_BUILT_MESSAGE]";
    orgbuilt = MessagesManager::add(orgbuilt, "ORGANISM_ID", std::to_string(organism));
    
    // Let the screenshot controller know to make a screenshot
    emitter->setChannel(SCREENSHOT_CHANNEL);
    emitter->send(orgbuilt.c_str(), (int)orgbuilt.length()+1);
    
    
    orgbuilt = MessagesManager::add(orgbuilt, "PARENT1", std::to_string(parent1));
    orgbuilt = MessagesManager::add(orgbuilt, "PARENT2", std::to_string(parent2));
    orgbuilt = MessagesManager::add(orgbuilt, "SIZE", std::to_string(size));
    orgbuilt = MessagesManager::add(orgbuilt, "GENOME", genome);
    orgbuilt = MessagesManager::add(orgbuilt, "MIND", mind);
    
    emitter->setChannel(EVOLVER_CHANNEL);
    emitter->send(orgbuilt.c_str(), (int)orgbuilt.length()+1);
}

////////////////////////////////////////////
////////////// INITIALIZATION //////////////
////////////////////////////////////////////

void BirthClinicController::connectModulesToObjects()
{
    // connect real modules to objects and store them into map and stack
    logger.debug("Connecting " +to_string(NUMBER_OF_MODULES)+" modules to objects");
    for (unsigned int i = 1; i < (NUMBER_OF_MODULES+1); i++)
    {
        logger.debug("Getting module " + std::string(MODULE_DEF_BASE_NAME) + std::to_string(i));
        Node * root = getFromDef(MODULE_DEF_BASE_NAME + std::to_string(i));
        if (root)
        {
            logger.debug("Got module " + std::string(MODULE_DEF_BASE_NAME) + std::to_string(i) + " from def, pointer is: "+boost::lexical_cast<std::string>(root));
            moduleMap[i] = new RoombotModule(root, i, ARENA_SIZE);
            logger.debug("Module Map i is: "+boost::lexical_cast<std::string>(moduleMap[i]));
            moduleMap[i]->toReserve();
            availableModules.push(i);
            logger.infoStream() << MODULE_DEF_BASE_NAME + std::to_string(i) + " INITIALIZED";
        }
        else
        {
            logger.debug("Module " + std::string(MODULE_DEF_BASE_NAME) + std::to_string(i) + " does not exist");
//            throw CREATE_LOCATEDEXCEPTION_INFO(MODULE_DEF_BASE_NAME + TO_STR(i) + " does not exist.");
        }
    }
}


////////////////////////////////////////////
///////////////// BUILDING /////////////////
////////////////////////////////////////////


int BirthClinicController::buildOrganism(CppnGenome genome, std::string mindGenome, id_t forcedId)
{
    std::auto_ptr<BuildPlan> buildPlan = builder->translateGenome(genome);
    
    if (buildPlan->size() > 1)
    {
        // add modules to organism
        if (availableModules.size() >= buildPlan->size())
        {
            // create an organism object and add the modules
            //if (countInRadius(position, CLINIC_SAFE_DISTANCE) == 0)
            if (isClinicFree())
            {
                rotate();
                
                size_t buildPlanSize = buildPlan->size();   // after creating the new organism the pointer to the build plan disappears
                
                id_t organismId;
                if (forcedId == 0)
                {
                    organismId = getNextOrganismId();
                }
                else
                {
                    organismId = forcedId;
                }
                
                BuildableOrganism * organism = new BuildableOrganism(genome.toString(), mindGenome, organismId, buildPlan, position);
                logger.noticeStream() << "New organism created: " << organism->getName();
                
                for(size_t i = 0; i < buildPlanSize; i++)
                {
                    // get module from map and remove from available stack
                    Module * moduleToAdd = moduleMap[availableModules.top()];
                    availableModules.pop();
                    
                    // update name in Webots
                    moduleToAdd->setName(std::to_string(organism->getId()), std::to_string(i));
                    logger.debugStream() << moduleToAdd->getDef() << " -> name has been updated to " << moduleToAdd->getName();
                    
                    // add it to the organism
                    organism->addModule(moduleToAdd);
                    logger.debugStream() << "module " << moduleToAdd->getId() << " added to "  << organism->getName();
                }
                // build organism, write relative ControllerArgs file and activate it (set controller and arguments fields on Webots)
                organism->build();
                organism->writeControllerArgsFile(simulationDateAndTime);
                organism->activate(simulationDateAndTime);
                
                logModuleCount("Organism built",availableModules.size());
                
                return buildPlanSize;
            }
            else
            {
                logger.debugStream() << "ORGANISM NOT CREATED: waiting for clinic to be free";
                return -2;
            }
        }
        else
        {
            logger.debugStream() << "ORGANISM NOT CREATED: not enough modules available";
            logModuleCount("Organism Aborted",availableModules.size());
            return -3;
        }
    }
    else
    {
        logger.debugStream() << "ORGANISM NOT CREATED: the build plan was empty or made of only one module";
        return -4;
    }
}

void BirthClinicController::logModuleCount(std::string message, int modules)
{
    ofstream file;
    file.open(RESULTS_PATH + simulationDateAndTime + "/birth-clinic.txt", ios::app);
    file << getTime() << " " << message << ": " << modules << " available modules left." << std::endl;
    file.close();
}



////////////////////////////////////////////
////////////// MAIN FUNCTIONS //////////////
////////////////////////////////////////////

BirthClinicController::BirthClinicController() : Supervisor(),
    logger(Logger::getInstance("BirthClinic"))
{
    logger.debug("Constructing Birth Clinic Controller");
    
    random = Utils::Random::getInstance();
    
    platform = getFromDef("CLINIC_PLATFORM");
    
    receiver = getReceiver(RECEIVER_NAME);
    receiver->setChannel(CLINIC_CHANNEL);
    emitter = getEmitter(EMITTER_NAME);
    
    // setup builder for shape encoding
    if (SHAPE_ENCODING == "CPPN")
    {
        builder = new Builder();
    }
    else
    {
        // set builder for other kind of genome
    }
    
    // update id for next organism
    nextOrganismId = 1;
}


BirthClinicController::~BirthClinicController()
{
    
}

/********************************
 ******* REBUILD ORGANISM *******
 ********************************/
bool BirthClinicController::rebuildOrganism(std::string message, int &buildTry) {
    id_t organismId;
    std::string genomeStr;
    std::string mindStr;
    readRebuildMessage(message, &organismId, &genomeStr, &mindStr);
    
    double time = getTime();
    while (getTime() - time < 3)
    {
        if(step(TIME_STEP) == -1){
            return false;
        }
    }
    
    std::istringstream stream(genomeStr);
    CppnGenome genome = builder->getGenomeFromStream(stream);
    int buildResponse = buildOrganism(genome, mindStr, organismId);
    
    if (buildResponse != -2)
    {
        buildTry = 0;
        return true;
    }
    else
    {
        buildTry++;
        step(TIME_STEP);
        return false;
    }
}

/**********************************
 ******* BUILD NEW ORGANISM *******
 **********************************/
bool BirthClinicController::buildOrganismFromMessage(std::string message, int &buildTry) {
    std::string genomeStr;
    std::string mindStr;
    id_t parent1, parent2;
    std::string fitness1, fitness2;
    readGenomeMessage(message, & genomeStr, & mindStr, & parent1, & parent2, & fitness1, & fitness2);
    
    std::istringstream stream(genomeStr);
    CppnGenome genome = builder->getGenomeFromStream(stream);
    int buildResponse = buildOrganism(genome, mindStr, 0);
    
    if (buildResponse > 0)
    {
        storePhilogenyOnFile(parent1, parent2, nextOrganismId-1, fitness1, fitness2);
        storeGenomeOnFile(nextOrganismId-1, genomeStr);
        storeMindGenomeOnFile(nextOrganismId-1, mindStr);
        
        sendOrganismBuiltMessage(parent1, parent2, nextOrganismId-1, buildResponse, genomeStr, mindStr);
        
        buildTry = 0;
        return true;
    } else if (buildResponse == -2) {
        buildTry++;
        // Clinic is still busy. Keep packets and wait a while.
    } else if (buildResponse == -3) {
        // There are not enough modules
        // Skip to the next shape
        buildTry++;
    } else if (buildResponse == -4) {
        // The build plan had only 1 or 2 modules
        // Skip to the next shape
        buildTry = 0;
    }
    return false;
}


void BirthClinicController::run()
{
    logger.debug("Running Birth Clinic Controller");
    
    TIME_STEP = getBasicTimeStep();

    receiver->enable(TIME_STEP);
    while (receiver->getQueueLength() > 0)
    {
        receiver->nextPacket();
    }
    
    
    /*******************************************
     ***** WAIT UNTIL ENVIRONMENT IS READY *****
     *******************************************/
    
    // wait unitil EnvironmentModifierController sais the environment is ok
    bool environmentOk = false;
    logger.debug("Waiting for environment to be ready");
    while (step(TIME_STEP) != -1 && !environmentOk)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            if (message.substr(0,24).compare("[ENVIRONMENT_OK_MESSAGE]") == 0)
            {
                environmentOk = true;
                simulationDateAndTime = MessagesManager::get(message, "SDAT");
                logger.debug("Environment ready");
            }
            receiver->nextPacket();
        }
    }
    
    /**************************************
     ******* MODULES INITIALIZATION *******
     **************************************/
    
    connectModulesToObjects();
    
    int buildTry = 0;
    int waitTime = -5;
    while (step(TIME_STEP) != -1)
    {
        /*******************************
         ******* MANAGE MESSAGES *******
         *******************************/
        double now = getTime();
        
        while (receiver->getQueueLength() > 0) {
            std::string message = (char*)receiver->getData();
            bool goNext = true;
            
            if (message.substr(0,26).compare("[UPDATE_AVAILABLE_MESSAGE]") == 0) {
                logger.debug("Received available message");
                std::string moduleDef = MessagesManager::get(message, "DEF");
                addModuleToReserve(moduleDef);
            } else if (message.substr(0,17).compare("[REBUILD_MESSAGE]") == 0) {
                logger.debug("Received rebuild message");
                goNext = rebuildOrganism(message, buildTry);
            } else if (message.substr(0,26).compare("[GENOME_TO_CLINIC_MESSAGE]") == 0) {
                logger.debug("Received genome to clinic message");
                    // Add to queue
                buildQueue.push_back(message);
                if(BIRTH_CLINIC_USE_QUEUE) {
                    logger.noticeStream() << BOLDRED << " Adding genome to queue, queue is now: " << buildQueue.size() << RESET;
                }
            } else {
                // Not enough modules in store, go to next shape
            }
            
            if(goNext) {
                receiver->nextPacket();
            }
        }

        if(now > waitTime + ROOMBOT_WAITING_TIME) {
            if(buildQueue.size() > 0 && availableModules.size() > BIRTH_CLINIC_MINIMUM_MODULES){
                std::string message = buildQueue.front();
                bool success = buildOrganismFromMessage(message,buildTry);
                if(success){
                    buildQueue.pop_front();
                    waitTime = now;
                    if(BIRTH_CLINIC_USE_QUEUE) {
                        logger.noticeStream() << BOLDRED << " Succesfully built organism, queue is now: " << buildQueue.size() << RESET;
                    } else {
                        buildQueue.clear();
                    }
                } else {
                    waitTime = now;
                }
            }
        }
    }
}