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
    orgbuilt = MessagesManager::add(orgbuilt, "PARENT1", std::to_string(parent1));
    orgbuilt = MessagesManager::add(orgbuilt, "PARENT2", std::to_string(parent2));
    orgbuilt = MessagesManager::add(orgbuilt, "ORGANISM_ID", std::to_string(organism));
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


int BirthClinicController::buildOrganism(CppnGenome genome, std::string mindGenome, id_t forcedId)
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
                organism->activate(simulationDateAndTime);
                return buildPlanSize;
            }
            else
            {
                //std::cout << "ORGANISM NOT CREATED: waiting for clinic to be free" << std::endl;
                return -2;
            }
        }
        else
        {
            std::cout << "ORGANISM NOT CREATED: not enough nodules available" << std::endl;
            return -3;
        }
    }
    else
    {
        std::cout << "ORGANISM NOT CREATED: the build plan was empty or made of only one module" << std::endl;
        return -3;
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


void BirthClinicController::run()
{
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
    while (step(TIME_STEP) != -1 && !environmentOk)
    {
        if(receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            if (message.substr(0,24).compare("[ENVIRONMENT_OK_MESSAGE]") == 0)
            {
                environmentOk = true;
                
                simulationDateAndTime = MessagesManager::get(message, "SDAT");
                
                boost::filesystem::path path(RESULTS_PATH + simulationDateAndTime);
                if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path)) {
                    throw std::runtime_error("Directory Already Existing");
                }
                boost::filesystem::create_directories(path);
            }
            receiver->nextPacket();
        }
    }
    
    
    /**************************************
     ******* MODULES INITIALIZATION *******
     **************************************/
    
    connectModulesToObjects();
    
    
    while (step(TIME_STEP) != -1)
    {
        /*******************************
         ******* MANAGE MESSAGES *******
         *******************************/
        
        if (receiver->getQueueLength() > 0)
        {
            std::string message = (char*)receiver->getData();
            
            
            /****************************************
             ******* UPDATE AVAILABLE MODULES *******
             ****************************************/
            
            if (message.substr(0,26).compare("[UPDATE_AVAILABLE_MESSAGE]") == 0)
            {
                std::string moduleDef = MessagesManager::get(message, "DEF");
                addModuleToReserve(moduleDef);
                receiver->nextPacket();
            }
            
            
            /********************************
             ******* REBUILD ORGANISM *******
             ********************************/
            
            if (message.substr(0,17).compare("[REBUILD_MESSAGE]") == 0)
            {
                id_t organismId;
                std::string genomeStr;
                std::string mindStr;
                readRebuildMessage(message, &organismId, &genomeStr, &mindStr);
                
                double time = getTime();
                while (getTime() - time < 3)
                {
                    step(TIME_STEP);
                }
                
                std::istringstream stream(genomeStr);
                CppnGenome genome = builder->getGenomeFromStream(stream);
                int buildResponse = buildOrganism(genome, mindStr, organismId);
                
                if (buildResponse != -2)
                {
                    receiver->nextPacket();
                }
            }
            
            
            /**********************************
             ******* BUILD NEW ORGANISM *******
             **********************************/
            
            if (message.substr(0,26).compare("[GENOME_TO_CLINIC_MESSAGE]") == 0)
            {
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
                    
                    receiver->nextPacket();
                }
                if (buildResponse == -3)
                {
                    // There are not enough modules, or the buildplan is too small
                    // Skip to the next shape
                    receiver->nextPacket();
                }
                if (buildResponse == -2)
                {
                    // Clinic is still busy. Keep packets and wait a while.
                }
            }
        }
    }
}