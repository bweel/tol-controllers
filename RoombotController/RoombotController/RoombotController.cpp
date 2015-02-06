#include "RoombotController.h"
#include "MatrixGenomeManager.h"
#include "SpeedFitness.h"

const std::string RoombotController::GPS_NAME = "GPS";
const std::string RoombotController::RESULTS_DIR = "Results";


/********************************************* CONSTRUCTORS *********************************************/

RoombotController::RoombotController()
{
    std::cout << "[" << worldModel.now << "] " << worldModel.robotName << ": Starting Controller"  << std::endl;
}

void RoombotController::initialise() {
    std::cout << "[" << worldModel.now << "] " << worldModel.robotName << ": Initialising"  << std::endl;
    initialiseWorldModel();
    
    evolverMessageHandler = MessageHandler(initialiseEmitter(EVOLVER_EMITTER_NAME),initialiseReceiver(EVOLVER_RECEIVER_NAME));
    deathMessageHandler = MessageHandler(NULL,initialiseReceiver(ROOMBOT_DEATH_RECEIVER_NAME));
    movementMessageHandler = MessageHandler(initialiseEmitter(ROOMBOT_EMITTER_NAME),initialiseReceiver(ROOMBOT_RECEIVER_NAME));
    
    
    evolverMessageHandler.setChannel(EVOLVER_CHANNEL);
    movementMessageHandler.setChannel(worldModel.organismId);
    deathMessageHandler.setChannel(DEATH_CHANNEL);
    
    gps = worldModel.iAmRoot() ? initialiseGPS(worldModel.TIME_STEP) : NULL;
    
    if(DEATH_SELECTION == "EVOLVER") {
        deathType = DEATH_SELECTION_BY_EVOLVER;
    } else if(DEATH_SELECTION == "TIME_TO_LIVE"){
        deathType = DEATH_SELECTION_BY_TIME_TO_LIVE;
    } else {
        std::cerr << "Unknown Death Selection Mechanism: " << DEATH_SELECTION << std::endl;
    }

    // lock connectors
    boost::property_tree::ptree connectorsNode = worldModel.parameters.get_child("Robot." + getName() + ".Connectors");
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, connectorsNode)
    {
        std::cout << "[" << worldModel.now << "] " << worldModel.robotName << ": Getting Connector: " << v.second.data()  << std::endl;
        Connector * connector = getConnector(v.second.data());
        if(!connector) {
            throw std::runtime_error("Device Not Found: "+to_string(v.second.data()));
        } else {
            connector->lock();
            connectors.push_back(connector);
        }
    }
    
    movementController = std::unique_ptr<MovementController>(new MovementController(worldModel, movementMessageHandler, this));
    movementController->initialise();

    if (worldModel.iAmRoot()) {
        learningController = std::unique_ptr<LearningController>(new LearningController(worldModel));
        adultFitnessMeasure = std::unique_ptr<FitnessMeasure>(new SpeedFitness(worldModel));
        
        Emitter *genomeEmitter = initialiseEmitter(ROOMBOT_GENOME_EMITTER_NAME);
        Receiver *genomeReceiver = initialiseReceiver(ROOMBOT_GENOME_RECEIVER_NAME);
        MessageHandler matingMessagesHandler(genomeEmitter,genomeReceiver);
        
        if (matingType == MATING_SELECTION_BY_EVOLVER)
        {
            matingMessagesHandler.getEmitter()->setRange(-1);
        }
        else if (matingType == MATING_SELECTION_BY_ORGANISMS)
        {
            matingMessagesHandler.getEmitter()->setRange(ORGANISM_GENOME_EMITTER_RANGE);
            matingMessagesHandler.setEmitterChannel(GENOME_EXCHANGE_CHANNEL);
        }
        matingMessagesHandler.setReceiverChannel(GENOME_EXCHANGE_CHANNEL);    // USED ONLY FOR DISTRIBUTED REPRODUCTION
        
        matingStrategy = MatingStrategy::getMatingStrategy(worldModel, matingMessagesHandler, evolverMessageHandler);
    }
    
    std::cout << "[" << worldModel.now << "] " << worldModel.robotName << ": " << "Finished initialising" << std::endl;
}


RoombotController::~RoombotController()
{
}

void RoombotController::initialiseWorldModel() {
    // read parameters tree
    worldModel.initialiseParameters(getControllerArguments());
    worldModel.TIME_STEP = getBasicTimeStep();
    worldModel.projectPath = getProjectPath();
    worldModel.robotType = worldModel.parameters.get<int>("Robot." + getName() + ".Type");
    worldModel.now = getTime();
    
    worldModel.fertile = false;
    worldModel.adult = false;
    worldModel.adultFitness = 0;
    
}

/********************************************* MINOR FUNCTIONS *********************************************/

// emitter initialization
Emitter * RoombotController::initialiseEmitter(std::string name)
{
    Emitter * emitter = getEmitter(name);
    if (!emitter) {
        throw std::runtime_error("Device Not Found: "+to_string(name));
    }
    
    return emitter;
}

// receiver initialization
Receiver * RoombotController::initialiseReceiver(std::string name)
{
    Receiver * receiver = getReceiver(name);
    if (!receiver) {
        throw std::runtime_error("Device Not Found: "+to_string(name));
    }
    receiver->enable(worldModel.TIME_STEP);

    return receiver;
}

// initialize GPS
GPS * RoombotController::initialiseGPS(double time_step)
{
    GPS * gps = getGPS(RoombotController::GPS_NAME);
    
    if (!gps) {
        throw std::runtime_error("Device Not Found: "+to_string(RoombotController::GPS_NAME));
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
    std::string message = "[ADULT_ANNOUNCEMENT]";
    message = MessagesManager::add(message, "ID", std::to_string(worldModel.organismId));
    
    evolverMessageHandler.send(message);
}


void RoombotController::sendFertileAnnouncement()
{
    std::string message = "[FERTILE_ANNOUNCEMENT]";
    message = MessagesManager::add(message, "ID", std::to_string(worldModel.organismId));

    evolverMessageHandler.send(message);
}


void RoombotController::sendFitnessUpdateToEvolver(double fitness)
{
    std::string message = "[FITNESS_UPDATE]";
    message = MessagesManager::add(message, "ID", std::to_string(worldModel.organismId));
    message = MessagesManager::add(message, "FITNESS", std::to_string(fitness));
    
    evolverMessageHandler.send(message);
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
        movementMessageHandler.send(message);
        return false;
    }
    
    startingTime = getTime();
    while (getTime() - startingTime < 1)
    {
        if(step(worldModel.TIME_STEP) == -1) {
            return connectorsOK;
        }
        
        while (movementMessageHandler.hasMessage())
        {
            std::string message = movementMessageHandler.receive();
            
            if (message.compare("[CONNECTORS_PROBLEM_MESSAGE]") == 0)
            {
                std::cout << getName() << " received connectors problem message" << std::endl;
                
                if (worldModel.iAmRoot())
                {
                    std::cout << getName() << " forwarded connectors problem" << std::endl;
                    
                    std::string message = "[CONNECTORS_PROBLEM_MESSAGE]";
                    movementMessageHandler.send(message);
                }
                return false;
            }
            
            movementMessageHandler.next();
        }
    }
    
    return connectorsOK;
}


bool RoombotController::checkFallenInside()
{
    bool positionOK = true;
    
    if (worldModel.iAmRoot())
    {
        double x = worldModel.position.x;
        double z = worldModel.position.z;
        double distanceFromCenter = sqrt((x*x) + (z*z));
        
        if (distanceFromCenter < 2)
        {
            std::cout << getName() << " detected fallen into cylinder problem" << std::endl;
            
            std::string message = "[CYLINDER_PROBLEM_MESSAGE]";
            movementMessageHandler.send(message);
            return false;
        }
    }
    else
    {
        double startingTime = getTime();
        while (getTime() - startingTime < 1) {
            if(step(worldModel.TIME_STEP) == -1) {
                return positionOK;
            }
            
            while(movementMessageHandler.hasMessage()) {
                std::string message = movementMessageHandler.receive();
                
                if (message.compare("[CYLINDER_PROBLEM_MESSAGE]") == 0) {
                    std::cout << getName() << " received fallen into cylinder problem" << std::endl;
                    
                    return false;
                }
                
                movementMessageHandler.next();
            }
        }
    }
    
    return positionOK;
}


void RoombotController::askToBeBuiltAgain()
{
    evolverMessageHandler.setChannel(CLINIC_CHANNEL);
    
    std::string message = "[REBUILD_MESSAGE]";
    message = MessagesManager::add(message, "ID", std::to_string(worldModel.organismId));
    message = MessagesManager::add(message, "GENOME", worldModel.bodyGenome);
    message = MessagesManager::add(message, "MIND", worldModel.mindGenome);
    
    evolverMessageHandler.send(message);
    evolverMessageHandler.setChannel(EVOLVER_CHANNEL);
}



/************************************************************* LIFE *************************************************************/

void RoombotController::life()
{
    worldModel.lifetimeStart = getTime();
    
    // ROOT MODULES BEHAVIOR
    if (worldModel.iAmRoot())
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
        double lastFitnessSent = 0;
        double lastFitnessUpdate = 0;
        double lastMating = 0;
        double lastEvolverUpdate = 0;
        worldModel.lifetimeStart = getTime();   // remember offset for time calculations
        adultFitnessMeasure->markStart();
        
        
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
                while(deathMessageHandler.hasMessage())
                {
                    std::string message = deathMessageHandler.receive();
                    deathMessageHandler.next();
                    if (message.compare(std::to_string(worldModel.organismId)) == 0 )
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
                if (worldModel.now - worldModel.lifetimeStart > worldModel.timeToLive)
                {
                    return; // end mature life (so die)
                }
            }
            
            
            /***************************************
             ************* CHECK ADULT *************
             ***************************************/
            
            if (!worldModel.adult && worldModel.now - worldModel.lifetimeStart > worldModel.infancyDuration)
            {
                worldModel.adult = true;
                if (worldModel.iAmRoot())
                {
                    sendAdultAnnouncement();
                }
            }
            
            
            /***************************************************************************
             ****** CHECK FERTILITY REQUIREMENT FOR MATING SELECTION BY ORGANISMS ******
             ***************************************************************************/
            
            if (matingType == MATING_SELECTION_BY_ORGANISMS)
            {
                if (!worldModel.fertile)
                {
                    double x = worldModel.position.x;
                    double z = worldModel.position.y;
                    double distanceFromCenter = sqrt((x*x) + (z*z));
                    
                    if (distanceFromCenter > FERTILITY_DISTANCE) {
                        
                        worldModel.fertile = true;
                        
                        sendFertileAnnouncement();
                        
                        storeFertilityOnFile();
                        std::cout << worldModel.organismId << " became fertile" << std::endl;
                    }
                }
            }
            
            
            /**************************
             ***** UPDATE FITNESS *****
             **************************/
            if (worldModel.adult && worldModel.now - lastFitnessUpdate > UPDATE_FITNESS_INTERVAL)
            {
                adultFitnessMeasure->markEnd();
                worldModel.adultFitness = adultFitnessMeasure->computeFitness();
                
                adultFitnessMeasure->markStart();
                lastFitnessUpdate = worldModel.now;
            }
            
            
            /*************************************
             ***** UPDATE FITNESS IN EVOLVER *****
             *************************************/
            if (matingType == MATING_SELECTION_BY_ORGANISMS && worldModel.adult)
            {
                if (worldModel.now - lastEvolverUpdate > UPDATE_FITNESS_IN_EVOLVER)
                {
                    sendFitnessUpdateToEvolver(worldModel.adultFitness);
                    lastEvolverUpdate = getTime();
                }
            }
            
            
            /**********************************
             ***** SEND GENOME TO EVOLVER *****
             **********************************/
            
            if (matingType == MATING_SELECTION_BY_EVOLVER)
            {
                if (worldModel.adult)
                {
                    if (worldModel.now - lastFitnessSent > SEND_FITNESS_TO_EVOLVER_INTERVAL)
                    {
                        // send genome and fitness to evolver
                        string message = "[GENOME_SPREAD_MESSAGE]";
                        message = MessagesManager::add(message, "ID", std::to_string(worldModel.organismId));
                        message = MessagesManager::add(message, "FITNESS", std::to_string(worldModel.adultFitness));
                        message = MessagesManager::add(message, "GENOME", worldModel.bodyGenome);
                        message = MessagesManager::add(message, "MIND", worldModel.mindGenome);
                        
                        evolverMessageHandler.send(message);
                        
                        // store in file
                        storeMatureLifeFitnessIntoFile(worldModel.adultFitness);
                        
                        lastFitnessSent = worldModel.now;
                    }
                }
            }
            
            
            /***********************************************
             ***** MATING USING SELECTION BY ORGANISMS *****
             ***********************************************/
            matingStrategy->findMates();
            if (worldModel.adult && worldModel.fertile) {
                if (worldModel.now - lastMating > INDIVIDUAL_MATING_TIME)
                {
                    matingStrategy->mate();
                    lastMating = worldModel.now;
                }
            }
            
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
                while(deathMessageHandler.hasMessage())
                {
                    std::string message = deathMessageHandler.receive();
                    deathMessageHandler.next();
                    if (message.compare(std::to_string(worldModel.organismId)) == 0 )
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
                if (worldModel.now - worldModel.lifetimeStart > worldModel.timeToLive)
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
    for(int i = 0; i < connectors.size(); i++) {
        connectors[i]->unlock();
    }
    
    double startingTime = getTime();
    while (getTime() - startingTime < 3)
    {
        movementController->normaliseMotors();
        if (step(worldModel.TIME_STEP) == -1) {
            return;
        }
    }
    
    std::string message = "[DEATH_ANNOUNCEMENT_MESSAGE]";
    message = MessagesManager::add(message, "ID", std::to_string(worldModel.organismId));
    evolverMessageHandler.send(message);
    
    evolverMessageHandler.setChannel(MODIFIER_CHANNEL);
    message = "[TO_RESERVE_MESSAGE]";
    message = MessagesManager::add(message, "NAME", getName());
    evolverMessageHandler.send(message);
    
    while (true) {
        movementController->normaliseMotors();
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
    // wait a timestep so we can debug
    std::string phase = "start";
    try
    {
        phase = "initialisation";
        initialise();
        
        updateWorldModel();
        
        // check if all modules are correctly locked to each other
        phase = "checking locks";
        if (!checkLocks())
        {
            if (worldModel.iAmRoot())
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
        updateWorldModel();
        if (checkInsideCylinder && !checkFallenInside())
        {
            if (worldModel.iAmRoot())
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
        updateWorldModel();
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