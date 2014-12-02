#include "RoombotController.h"
#include "MatrixGenomeManager.h"

const std::string RoombotController::GPS_NAME = "GPS";
const std::string RoombotController::RESULTS_DIR = "Results";


/********************************************* CONSTRUCTORS *********************************************/

RoombotController::RoombotController() {
}

void RoombotController::initialise() {
    std::cout << "[" << getTime() << "] " << getName() << ": " << "Starting Controller, timestep: " << worldModel.TIME_STEP  << std::endl;
    initialiseWorldModel();
    
    gps = worldModel.iAmRoot() ? initialiseGPS(worldModel.TIME_STEP) : NULL;
    parentSelectionMechanism = ParentSelectionMechanism::getParentSelectionMechanism();
    
    if(DEATH_SELECTION == "EVOLVER") {
        deathType = DEATH_SELECTION_BY_EVOLVER;
    } else if(DEATH_SELECTION == "TIME_TO_LIVE"){
        deathType = DEATH_SELECTION_BY_TIME_TO_LIVE;
    } else {
        std::cerr << "Unknown Death Selection Mechanism: " << DEATH_SELECTION << std::endl;
    }
    
    deathReceiver = getReceiver(ROOMBOT_DEATH_RECEIVER_NAME);
    deathReceiver->setChannel(DEATH_CHANNEL);
    deathReceiver->enable(worldModel.TIME_STEP);
    while (deathReceiver->getQueueLength() > 0)
    {
        deathReceiver->nextPacket();
    }
    
    genomeReceiver = getReceiver(ROOMBOT_GENOME_RECEIVER_NAME);
    genomeEmitter = getEmitter(ROOMBOT_GENOME_EMITTER_NAME);
    
    genomeReceiver->disable(); // always disabled for non-root modules, will become enabled for the root when organism becomes fertile
    
    // lock connectors
    boost::property_tree::ptree connectorsNode = worldModel.parameters.get_child("Robot." + getName() + ".Connectors");
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, connectorsNode)
    {
        Connector * connector = getConnector(v.second.data());
        connector->lock();
        connectors.push_back(connector);
    }

    Emitter *emitter = initialiseEmitter();
    Receiver *receiver = initialiseReceiver(worldModel.TIME_STEP);
    MessageHandler movementMessagesHandler(emitter,receiver);
    movementMessagesHandler.setChannel(worldModel.organismId);
    
    movementController = std::unique_ptr<MovementController>(new MovementController(worldModel, movementMessagesHandler));
    
    if (worldModel.iAmRoot()) {
        learningController = std::unique_ptr<LearningController>(new LearningController(worldModel));
        
        if (matingType == MATING_SELECTION_BY_EVOLVER)
        {
            genomeEmitter->setRange(-1);
            genomeEmitter->setChannel(EVOLVER_CHANNEL);
        }
        else if (matingType == MATING_SELECTION_BY_ORGANISMS)
        {
            genomeEmitter->setRange(ORGANISM_GENOME_EMITTER_RANGE);
            genomeEmitter->setChannel(GENOME_EXCHANGE_CHANNEL);
        }
        genomeReceiver->setChannel(GENOME_EXCHANGE_CHANNEL);    // USED ONLY FOR DISTRIBUTED REPRODUCTION
    }
}


RoombotController::~RoombotController()
{
}

void RoombotController::initialiseWorldModel() {
    // read parameters tree
    worldModel.initialiseParameters(getControllerArguments());
    worldModel.projectPath = getProjectPath();
    worldModel.robotType = worldModel.parameters.get<int>("Robot." + getName() + ".Type");
}

/********************************************* MINOR FUNCTIONS *********************************************/

// emitter initialization
Emitter * RoombotController::initialiseEmitter()
{
    Emitter * emitter = getEmitter(ROOMBOT_EMITTER_NAME);
    if (!emitter) {
        throw std::runtime_error("Device Not Found");
    }
//    emitter->setChannel(channel);
    return emitter;
}

// receiver initialization
Receiver * RoombotController::initialiseReceiver(double time_step)
{
    Receiver * receiver = getReceiver(ROOMBOT_RECEIVER_NAME);
    if (!receiver) {
        throw std::runtime_error("Device Not Found");
    }
    receiver->enable(time_step);
//    receiver->setChannel(channel);

    return receiver;
}

// initialize GPS
GPS * RoombotController::initialiseGPS(double time_step)
{
    GPS * gps = getGPS(RoombotController::GPS_NAME);
    
    if (!gps) {
        throw std::runtime_error("Device Not Found");
    }
    
    gps->enable(time_step);

    return gps;
}

// get GPS position
Vector3<double> RoombotController::getGPSPosition()
{
    const double * values = gps->getValues();
    return Vector3<double>(values[0], 0.0, values[2]);
}

void RoombotController::storeMatureLifeFitnessIntoFile(double fitness)
{
    ofstream fitnessFile;
    fitnessFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/organism_" + std::to_string(worldModel.organismId) + "/mature_life_fitness.txt", ios::app);
    fitnessFile << getTime() << " " << fitness << std::endl;
    fitnessFile.close();
}


void RoombotController::storeFertilityOnFile()
{
    ofstream fitnessFile;
    fitnessFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/fertility.txt", ios::app);
    fitnessFile << getTime() << " " << worldModel.organismId << std::endl;
    fitnessFile.close();
}


void RoombotController::storeRebuild(std::string message)
{
    ofstream rebuildFile;
    rebuildFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/rebuild.txt", ios::app);
    rebuildFile << getTime() << " " << worldModel.organismId << " " << message << std::endl;
    rebuildFile.close();
}


void RoombotController::storeProblem(std::string message, std::string phase)
{
    ofstream problemFile;
    problemFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/problems.txt", ios::app);
    problemFile << getTime() << " " << getName() << " PROBLEM IN " << phase << ": " << message << std::endl;
    problemFile.close();
}

void RoombotController::storeMatingLocation(std::string partner, std::string location)
{
    ofstream matingLocationFile;
    matingLocationFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/mating-locations.txt", ios::app);
    matingLocationFile << getTime() << " " << getName() << " mated with " << partner << " at " << location << std::endl;
    matingLocationFile.close();
}

void RoombotController::storeReproductionLocation(std::string partner, std::string location)
{
    ofstream reproductionLocationFile;
    reproductionLocationFile.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/reproduction-locations.txt", ios::app);
    reproductionLocationFile << getTime() << " " << getName() << " chose as partner: " << partner << " at " << location << std::endl;
    reproductionLocationFile.close();
}

void RoombotController::logNumberOfReceivedGenomes()
{
    ofstream file;
    file.open(RESULTS_PATH + worldModel.simulationDateAndTime + "/organism_" + std::to_string(worldModel.organismId) + "/received_genomes.txt", ios::app);
    file << getTime() << " " << organismsToMateWith.size() << " ";
    for (int i = 0; i < organismsToMateWith.size(); i++)
    {
        file << organismsToMateWith[i].getName() << " ";
    }
    file << std::endl;
    file.close();
}


bool RoombotController::allConnectorsOK()
{
    for (int i = 0; i < connectors.size(); i++)
    {
        if (connectors[i]->getPresence() != 1)
        {
            return false;
        }
    }
    return true;
}


void RoombotController::enableAllConnectors()
{
    for (int i = 0; i < connectors.size(); i++)
    {
        connectors[i]->enablePresence(worldModel.TIME_STEP/4);
    }
}


void RoombotController::disableAllConnectors()
{
    for (int i = 0; i < connectors.size(); i++)
    {
        connectors[i]->disablePresence();
    }
}

void RoombotController::sendAdultAnnouncement()
{
    int backupChannel = _emitter->getChannel();
    _emitter->setChannel(EVOLVER_CHANNEL);
    std::string message = "[ADULT_ANNOUNCEMENT]";
    message = MessagesManager::add(message, "ID", std::to_string(organismId));
    _emitter->send(message.c_str(), (int)message.length()+1);
    _emitter->setChannel(backupChannel);
}


void RoombotController::sendFertileAnnouncement()
{
    int backupChannel = _emitter->getChannel();
    _emitter->setChannel(EVOLVER_CHANNEL);
    std::string message = "[FERTILE_ANNOUNCEMENT]";
    message = MessagesManager::add(message, "ID", std::to_string(organismId));
    _emitter->send(message.c_str(), (int)message.length()+1);
    _emitter->setChannel(backupChannel);
}


void RoombotController::sendFitnessUpdateToEvolver(double fitness)
{
    int backupChannel = _emitter->getChannel();
    _emitter->setChannel(EVOLVER_CHANNEL);
    std::string message = "[FITNESS_UPDATE]";
    message = MessagesManager::add(message, "ID", std::to_string(organismId));
    message = MessagesManager::add(message, "FITNESS", std::to_string(fitness));
    _emitter->send(message.c_str(), (int)message.length()+1);
    _emitter->setChannel(backupChannel);
}


/******************************************* BEFORE STARTING *******************************************/

bool RoombotController::checkLocks()
{
    enableAllConnectors();
    
    double startingTime = getTime();
    bool connectorsOK = true;
    while (getTime() - startingTime < 2)
    {
        if (step(worldModel.TIME_STEP) != -1)
        {
            if (!allConnectorsOK())
            {
                connectorsOK = false;
            }
        }
    }
    
    disableAllConnectors();
    
    if (!connectorsOK)
    {
        std::cout << getName() << " detected connectors problem" << std::endl;
        
        std::string message = "[CONNECTORS_PROBLEM_MESSAGE]";
        _emitter->send(message.c_str(), (int)message.length()+1);
        return false;
    }
    
    startingTime = getTime();
    while (getTime() - startingTime < 1)
    {
        if(step(worldModel.TIME_STEP) == -1) {
            return connectorsOK;
        }
        
        while (_receiver->getQueueLength() > 0)
        {
            std::string message = (char*)_receiver->getData();
            
            if (message.compare("[CONNECTORS_PROBLEM_MESSAGE]") == 0)
            {
                std::cout << getName() << " received connectors problem message" << std::endl;
                
                if (isRoot())
                {
                    std::cout << getName() << " forwarded connectors problem" << std::endl;
                    
                    std::string message = "[CONNECTORS_PROBLEM_MESSAGE]";
                    _emitter->send(message.c_str(), (int)message.length()+1);
                }
                return false;
            }
            
            _receiver->nextPacket();
        }
    }
    
    return connectorsOK;
}


bool RoombotController::checkFallenInside()
{
    bool positionOK = true;
    
    if (isRoot())
    {
        const double * position = _gps->getValues();
        double x = position[0];
        double z = position[2];
        double distanceFromCenter = sqrt((x*x) + (z*z));
        
        if (distanceFromCenter < 2)
        {
            std::cout << getName() << " detected fallen into cylinder problem" << std::endl;
            
            std::string message = "[CYLINDER_PROBLEM_MESSAGE]";
            _emitter->send(message.c_str(), (int)message.length()+1);
            return false;
        }
    }
    else
    {
        double startingTime = getTime();
        while (getTime() - startingTime < 1)
        {
            if(step(worldModel.TIME_STEP) == -1) {
                return positionOK;
            }
            
            while(_receiver->getQueueLength() > 0)
            {
                std::string message = (char*)_receiver->getData();
                
                if (message.compare("[CYLINDER_PROBLEM_MESSAGE]") == 0)
                {
                    std::cout << getName() << " received fallen into cylinder problem" << std::endl;
                    
                    return false;
                }
                
                _receiver->nextPacket();
            }
        }
    }
    
    return positionOK;
}


void RoombotController::askToBeBuiltAgain()
{
    _emitter->setChannel(CLINIC_CHANNEL);
    
    std::string message = "[REBUILD_MESSAGE]";
    message = MessagesManager::add(message, "ID", std::to_string(organismId));
    message = MessagesManager::add(message, "GENOME", genome);
    message = MessagesManager::add(message, "MIND", mindGenome);
    
    _emitter->send(message.c_str(), (int)message.length()+1);
}



/************************************************************* LIFE *************************************************************/

void RoombotController::life()
{
    worldModel.lifeStartingTime = getTime();
    
    // ROOT MODULES BEHAVIOR
    
    if (isRoot())
    {
        
        /***********************************
         *********** PREPARATION ***********
         ***********************************/
        
#ifdef DEBUG_CONTROLLER
        std::cout << "[" << getTime() << "] " << getName() << ": We are root initialize things" << std::endl;
#endif
        
        // two first steps (one more than non-root modules)
        if (step(worldModel.TIME_STEP) == -1)
            return;
        if (step(worldModel.TIME_STEP) == -1)
            return;
        
        
        // set variables for cycle
        bool flag = false;
        std::pair<double, std::string> fitness(0.0, "");
        double lastFitnessSent = 0;
        double lastFitnessUpdate = 0;
        double lastMating = 0;
        double lastEvolverUpdate = 0;
        worldModel.lifetimeStart = getTime();   // remember offset for time calculations
        _position_start = getGPSPosition();
        
        
        /************************************
         ************ LIFE CYCLE ************
         ************************************/
        
        while (step(worldModel.TIME_STEP) != -1)
        {
            updateWorldModel();
            
            
            /********************************************
             ***** CHECK DEATH BY EVOLVER SELECTION *****
             ********************************************/
            
            if (deathType == DEATH_SELECTION_BY_EVOLVER)
            {
                while(deathReceiver->getQueueLength() > 0)
                {
                    std::string message = (char*)deathReceiver->getData();
                    deathReceiver->nextPacket();
                    if (message.compare(std::to_string(organismId)) == 0 )
                    {
                        return; // end mature life (so die)
                    }
                }
            }
            
            
            /***************************************
             ***** CHECK DEATH BY TIME TO LIVE *****
             ***************************************/
            
            if (deathType == DEATH_SELECTION_BY_TIME_TO_LIVE)
            {
                if (now - worldModel.lifeStartingTime > timeToLive)
                {
                    return; // end mature life (so die)
                }
            }
            
            
            /***************************************
             ************* CHECK ADULT *************
             ***************************************/
            
            if (!adult)
            {
                if (now - worldModel.lifeStartingTime > infancyDuration)
                {
                    adult = true;
                    if (isRoot())
                    {
                        sendAdultAnnouncement();
                    }
                }
            }
            
            
            /***************************************************************************
             ****** CHECK FERTILITY REQUIREMENT FOR MATING SELECTION BY ORGANISMS ******
             ***************************************************************************/
            
            if (matingType == MATING_SELECTION_BY_ORGANISMS)
            {
                if (!fertile)
                {
                    const double * position = _gps->getValues();
                    double x = position[0];
                    double z = position[2];
                    double distanceFromCenter = sqrt((x*x) + (z*z));
                    
                    if (distanceFromCenter > FERTILITY_DISTANCE)
                    {
                        fertile = true;
                        if (isRoot())
                        {
                            sendFertileAnnouncement();
                        }
                        
                        genomeReceiver->enable(worldModel.TIME_STEP);
                        while (genomeReceiver->getQueueLength() > 0)
                        {
                            genomeReceiver->nextPacket();
                        }
                        
                        storeFertilityOnFile();
                        std::cout << organismId << " became fertile" << std::endl;
                    }
                }
            }
            
            
            /**************************
             ***** UPDATE FITNESS *****
             **************************/
            
            if (adult)
            {
                if (now - lastFitnessUpdate > UPDATE_FITNESS_INTERVAL)
                {
                    // compute fitness
                    _time_end = getTime();
                    _position_end = _get_gps();
                    fitness = _compute_fitness((_time_end - _time_start), (_position_end - _position_start));
                    
                    // reset time, position and lastFitnessSent for next fitness evaluation
                    _time_start = getTime();
                    _position_start = _get_gps();
                    lastFitnessUpdate = getTime();
                }
            }
            
            
            /*************************************
             ***** UPDATE FITNESS IN EVOLVER *****
             *************************************/
            
            if (matingType == MATING_SELECTION_BY_ORGANISMS)
            {
                if (adult)
                {
                    if (now - lastEvolverUpdate > UPDATE_FITNESS_IN_EVOLVER)
                    {
                        sendFitnessUpdateToEvolver(fitness.first);
                        lastEvolverUpdate = getTime();
                    }
                }
            }
            
            
            /**********************************
             ***** SEND GENOME TO EVOLVER *****
             **********************************/
            
            if (matingType == MATING_SELECTION_BY_EVOLVER)
            {
                if (adult)
                {
                    if (now - lastFitnessSent > SEND_FITNESS_TO_EVOLVER_INTERVAL)
                    {
                        // send genome and fitness to evolver
                        string message = "[GENOME_SPREAD_MESSAGE]";
                        message = MessagesManager::add(message, "ID", std::to_string(organismId));
                        message = MessagesManager::add(message, "FITNESS", std::to_string(fitness.first));
                        message = MessagesManager::add(message, "GENOME", genome);
                        message = MessagesManager::add(message, "MIND", mindGenome);
                        genomeEmitter->send(message.c_str(), (int)message.length()+1);
                        
                        // store in file
                        storeMatureLifeFitnessIntoFile(fitness.first);
                        
                        lastFitnessSent = getTime();
                    }
                }
            }
            
            
            /***********************************************
             ***** MATING USING SELECTION BY ORGANISMS *****
             ***********************************************/
            
            matingStrategy->findMates();
            
            /*******************
             *** STEP MOTORS ***
             *******************/
            movementController->step();
        } // while
        
        learningController->finalise();
    }
    // NON-ROOT MODULES BEHAVIOR
    
    else
    {
        /***********************************
         *********** PREPARATION ***********
         ***********************************/

        // one first step
        if (step(worldModel.TIME_STEP) == -1)
            return;
        
        
        /************************************
         ************ LIFE CYCLE ************
         ************************************/
        
        while (step(worldModel.TIME_STEP) != -1)
        {
            
            updateWorldModel();
            /********************************************
             ***** CHECK DEATH BY EVOLVER SELECTION *****
             ********************************************/
            
            if (deathType == DEATH_SELECTION_BY_EVOLVER)
            {
                while (deathReceiver->getQueueLength() > 0)
                {
                    std::string message = (char*)deathReceiver->getData();
                    deathReceiver->nextPacket();
                    if (message.compare(std::to_string(organismId)) == 0 )
                    {
                        return; // end mature life (so die)
                    }
                }
            }
            
            
            /***************************************
             ***** CHECK DEATH BY TIME TO LIVE *****
             ***************************************/
            
            else if (deathType == DEATH_SELECTION_BY_TIME_TO_LIVE)
            {
                if (getTime() - lifeStartingTime > timeToLive)
                {
                    return; // and die
                }
            }
            
            
            /*******************
             *** STEP MOTORS ***
             *******************/
            movementController->step();
        }
    }
    
}



/********************************************* DEATH *********************************************/

void RoombotController::death()
{
    if(isRoot()) {
        genomeReceiver->disable();
    }
    
    for(int i = 0; i < connectors.size(); i++) {
        connectors[i]->unlock();
    }
    
    double startingTime = getTime();
    while (getTime() - startingTime < 3)
    {
        for(int i = 0; i < numMotors; i++) {
            _set_motor_position(i, 0);
        }
        
        if (step(worldModel.TIME_STEP) == -1) {
            return;Ï
        }
    }
    
    _emitter->setChannel(EVOLVER_CHANNEL);
    std::string message = "[DEATH_ANNOUNCEMENT_MESSAGE]";
    message = MessagesManager::add(message, "ID", std::to_string(organismId));
    _emitter->send(message.c_str(), (int)message.length()+1);
    
    _emitter->setChannel(MODIFIER_CHANNEL);
    message = "[TO_RESERVE_MESSAGE]";
    message = MessagesManager::add(message, "NAME", getName());
    _emitter->send(message.c_str(), (int)message.length()+1);
    
    while (true) {
        for(int i = 0; i < numMotors; i++) {
            _set_motor_position(i, 0);
        }
        if (step(worldModel.TIME_STEP) == -1) {
            return;
        }
    }
}

/************************************* UPDATE WORLD MODEL **************************************/

void RoombotController::updateWorldModel() {
    worldModel.now = getTime();
    worldModel.position = getGPSPosition();
}

/********************************************* RUN *********************************************/

void RoombotController::run()
{
    std::string phase = "start";
    try
    {
        std::string phase = "initialisation";
        initialise();
        
        // check if all modules are correctly locked to each other
        phase = "checking locks";
        if (!checkLocks())
        {
            if (isRoot())
            {
                askToBeBuiltAgain();
                storeRebuild("connection problem");
            }
            phase = "death";
            death();
            return;
        }
        
        phase = "waiting";
        // wait for module to not move after sliding down from clinic
        double startingTime = getTime();
        while (getTime() - startingTime < ROOMBOT_WAITING_TIME)
        {
            if (step(worldModel.TIME_STEP) == -1)
            {
                return;
            }
        }
        
        phase = "checking inside cylinder";
        // check if it fell inside the cylinder
        if (checkInsideCylinder && !checkFallenInside())
        {
            if (isRoot())
            {
                askToBeBuiltAgain();
                storeRebuild("fallen into cylinder");
            }
            phase = "death";
            death();
            return;
        }
        
        std::cout << getName() << " STARTS LIFE" << std::endl;
        
        phase = "life";
        life();
        
        std::cout << getName() << " STARTS DEATH" << std::endl;
        
        phase = "death";
        death();
    }
    catch (int err) {
        std::string message = "error number" + std::to_string(err);
        std::cerr << "PROBLEM in " << phase << ": " << message << std::endl;
        storeProblem(message, phase);
    }
    catch (LocatedException e) {
        std::string message = e.what();
        std::cerr << "PROBLEM in " << phase << ": " << message << std::endl;
        storeProblem(message, "DEATH");
    }
    catch (std::runtime_error e) {
        std::string message = e.what();
        std::cerr << "RUNTIME_ERROR in " << phase << ": " << message << std::endl;
        storeProblem(message, phase);
    }
    catch (std::exception e) {
        std::string message = e.what();
        std::cerr << "PROBLEM in " << phase << ": " << message << std::endl;
        storeProblem(message, phase);
    }
    catch (...) {
        std::string message = "something else";
        std::cerr << "PROBLEM in " << phase << ": " << message << std::endl;
        storeProblem(message, phase);
    }
}