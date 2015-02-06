#ifndef ROOMBOT_CONTROLLER_H
#define	ROOMBOT_CONTROLLER_H

//#define DEBUG_CONTROLLER
//#define DEBUG_TIMING

#include "Organism.h"
#include "Defines.h"
#include "ParametersReader.h"
#include "MessagesManager.h"
#include "MessageHandler.h"
#include "ParentSelectionMechanism.h"
#include "BestTwoParentSelection.h"
#include "BinaryTournamentParentSelection.h"
#include "RandomSelection.h"
#include "WorldModel.h"
#include "MatingStrategy.h"
#include "MessageHandler.h"

#include "LearningAlgorithm.h"
#include "HyperNEAT.h"
#include "RL_PoWER.h"
#include "SplineNeat.h"

#include "MovementController.h"

#include <webots/Robot.hpp>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>

#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include <stdexcept>
#include <ctime>


using namespace webots;

class RoombotController : public Robot {
public:
    static const std::string GPS_NAME;              // name of the GPS
    static const std::string RESULTS_DIR;           // results directory
    
    RoombotController();             // contructor
    virtual ~RoombotController();    // deconstructor
    
    void run();                 // robot behavior
    
private:
    // ATTRIBUTES
    int EVOLVER_CHANNEL = ParametersReader::get<int>("EVOLVER_CHANNEL");
    int CLINIC_CHANNEL = ParametersReader::get<int>("CLINIC_CHANNEL");
    int DEATH_CHANNEL = ParametersReader::get<int>("DEATH_CHANNEL");
    int MODIFIER_CHANNEL = ParametersReader::get<int>("MODIFIER_CHANNEL");
    int GENOME_EXCHANGE_CHANNEL = ParametersReader::get<int>("GENOME_EXCHANGE_CHANNEL");
    
    double ORGANISM_GENOME_EMITTER_RANGE = ParametersReader::get<double>("ORGANISM_GENOME_EMITTER_RANGE");
    
    int SEND_FITNESS_TO_EVOLVER_INTERVAL = ParametersReader::get<int>("SEND_FITNESS_TO_EVOLVER_INTERVAL");  // FOR CENTRALIZED REPRODUCTION
    int SPREAD_FITNESS_INTERVAL = ParametersReader::get<int>("SPREAD_FITNESS_INTERVAL");                    // FOR DISTRIBUTED REPRODUCTION
    int INDIVIDUAL_MATING_TIME = ParametersReader::get<int>("INDIVIDUAL_MATING_TIME");                      // FOR DISTRIBUTED REPRODUCTION
    int ROOMBOT_WAITING_TIME = ParametersReader::get<int>("ROOMBOT_WAITING_TIME");
    int UPDATE_FITNESS_IN_EVOLVER = ParametersReader::get<int>("MATING_TIME");      // FOR DISTRIBUTED REPRODUCTION
    int UPDATE_FITNESS_INTERVAL = ParametersReader::get<int>("UPDATE_FITNESS_INTERVAL");
    
    double FERTILITY_DISTANCE = ParametersReader::get<double>("FERTILITY_DISTANCE");   
    
    std::string DEATH_SELECTION = ParametersReader::get<std::string>("DEATH_SELECTION");
    
    bool checkInsideCylinder = ParametersReader::get<bool>("CHECK_INSIDE_CYLINDER");
    
    WorldModel worldModel;
    
    MatingType matingType;
    DeathType deathType;
    
    std::unique_ptr<MovementController> movementController;
    std::unique_ptr<LearningController> learningController;
    std::unique_ptr<FitnessMeasure> adultFitnessMeasure;
    
    std::unique_ptr<MatingStrategy> matingStrategy;
    
    MessageHandler evolverMessageHandler;
    MessageHandler movementMessageHandler;
    MessageHandler deathMessageHandler;
    
    GPS * gps;                                 // GPS
    
    std::vector<Connector *> connectors = std::vector<Connector *>();
    
    // PRIVATE METHODS
    void initialise();
    void initialiseWorldModel();
    
    Emitter * _init_emitter_clinic(int);
    Emitter * initialiseEmitter(std::string name);
    Receiver * initialiseReceiver(std::string name);
    
    GPS * initialiseGPS(double);

    /**
     * Gets the current position from GPS ignoring the Y axis
     * In our Webots simulations the Y axis is the height
     * @return
     */
    Vector3<double> getGPSPosition();
    
    bool allConnectorsOK();
    
    void enableAllConnectors();
    
    void disableAllConnectors();
    
    void storeMatureLifeFitnessIntoFile(double fitness);
    
    void storeRebuild(std::string message);
    
    void storeFertilityOnFile();
    
    void logNumberOfReceivedGenomes();
    
    void storeProblem(std::string message, std::string phase);
    
    double getRealFitness(double fitness);
    
    void logGPS();
    
    // before starting
    bool checkLocks();
    
    bool checkFallenInside();
    
    void life();
    
    // deactivate
    void death();
    
    void updateWorldModel();
    
    void askToBeBuiltAgain();
    
    void sendAdultAnnouncement();
    
    void sendFertileAnnouncement();
    
    void sendFitnessUpdateToEvolver(double fitness);
};

#endif	/* ROOMBOT_CONTROLLER_H */