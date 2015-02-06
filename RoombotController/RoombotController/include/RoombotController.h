#ifndef ROOMBOT_CONTROLLER_H
#define	ROOMBOT_CONTROLLER_H

//#define DEBUG_CONTROLLER
//#define DEBUG_TIMING

#include "Organism.h"
#include "Defines.h"
#include "ParametersReader.h"
#include "MessagesManager.h"
#include "ParentSelectionMechanism.h"
#include "BestTwoParentSelection.h"
#include "BinaryTournamentParentSelection.h"
#include "RandomSelection.h"

#include "EVAlgorithm.h"
#include "HyperNEAT.h"
#include "RL_PoWER.h"
#include "SplineNeat.h"

#include "MyMath.h"
#include "Vector_3.h"

#include <webots/Robot.hpp>
#include <vector>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>

#include <stdexcept>
#include <ctime>


using namespace webots;

typedef std::vector<double> dvector;
typedef std::vector<dvector> doubledvector;

class RoombotController : public Robot
{
    
public:
    
    // CONSTANTS
    
    static const std::string GPS_NAME;              // name of the GPS
    
    static const std::string BUFFER_NAME;           // name of the buffer
    static const std::string RESULTS_DIR;           // results directory
    
    static const int GPS_LOG_PERIOD;                // gps logging period (a print every tot steps)
    
    
    // TYPES
    
    enum Module_Types       // type of the single modules
    {
        M_STATIC = 0x00,    // static
        M_JOINT = 0x01,     // slave
        M_ROOT = 0x02       // master
    };
    
    // PUBLIC METHODS
    
    RoombotController();             // contructor
    
    virtual ~RoombotController();    // deconstructor
    
    void run();                 // robot behavior
    
    
    
private:
    
    // ATTRIBUTES
    
    int CLINIC_CHANNEL = ParametersReader::get<int>("CLINIC_CHANNEL");
    int DEATH_CHANNEL = ParametersReader::get<int>("DEATH_CHANNEL");
    int EVOLVER_CHANNEL = ParametersReader::get<int>("EVOLVER_CHANNEL");
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
    
    std::string PARENT_SELECTION = ParametersReader::get<std::string>("PARENT_SELECTION");
    std::string MATING_SELECTION = ParametersReader::get<std::string>("MATING_SELECTION");
    std::string DEATH_SELECTION = ParametersReader::get<std::string>("DEATH_SELECTION");
    
    bool checkInsideCylinder = ParametersReader::get<bool>("CHECK_INSIDE_CYLINDER");
    std::string simulationDateAndTime;
    
    double TIME_STEP;
    
    bool fertile;
    bool adult;
    
    MatingType matingType;
    DeathType deathType;
    
    boost::property_tree::ptree * _parameters;  // parameters tree
    
    std::string _name;                          // whole robot's name
    int organismId;
    
    std::string genome;
    std::string mindGenome;
    
    std::string logDirectory;                   // directory of the log file
    
    unsigned int _seed;
    
    std::size_t _r_index;                       // index of the first module
    std::size_t _r_index_root;                  // index of the root module
    std::size_t organismSize;                   // number of modules
    
    std::size_t _m_index;                       // module's index
    int _m_type;                                // module's type (0=static, 1=slave, 2=master)
    
    double _time_offset;                        // time - offset
    
    int _ev_type;                               // algorithm (1=HyperNEAT, 2=RL PoWER)
    std::size_t _ev_step;                       // current evaluation step
    std::size_t _ev_steps_recovery;             // time steps without fitness
    std::size_t _ev_steps_total_infancy;        // time steps to wait before evaluation during infancy
    unsigned int infancyDuration;            // total time of infancy
    unsigned int timeToLive;              // total time to live
    unsigned int totalEvaluations;              // total number of evaluations
    unsigned int generation;                    // current generation
    double _ev_angular_velocity;                // speed of the motors
    
    double learningTimeStart;                         // time - start
    transforms::Vector_3 learningPositionStart;       // list of transformations
    
    double adultFitnessTimeStart;
    transforms::Vector_3 adultFitnessPositionStart;         // from start to end
    
    unsigned int numMotors;                     // number of motors
    double motorRange;                          // range of motors
    
    EVAlgorithm * _algorithm;                   // learning algorithm
    
    GPS * _gps;                                 // GPS
    Emitter * _emitter;                         // emitter
    Receiver * _receiver;                       // receiver
    Receiver * deathReceiver;
    Emitter * genomeEmitter;
    Receiver * genomeReceiver;
    
    std::vector<Organism> organismsToMateWith = std::vector<Organism>();
    
    Motor ** _motors;                           // motors
    
    std::ofstream gpsLog;                       // GPS log
    
    std::vector<Connector *> connectors = std::vector<Connector *>();
    
    ParentSelectionMechanism * parentSelectionMechanism;
    
    // PRIVATE METHODS
    boost::property_tree::ptree * _init_parameters(const std::string &);
    
    GPS * _init_gps(double);
    Emitter * _init_emitter(int);
    Emitter * _init_emitter_clinic(int);
    Receiver * _init_receiver(double, int);
    Motor ** _init_motors(double);
    
    std::string _init_directory(const std::string &, const std::string &, const std::string &, const std::string &logdir, unsigned int);
    transforms::Vector_3 _init_shape_size(const boost::property_tree::ptree &);
    std::vector<transforms::Vector_3> initialiseModuleMapping(std::size_t, std::size_t, const std::string &, const boost::property_tree::ptree &);
    
    void sendAngles(doubledvector anglesOut);
    void sendAngles(size_t index, dvector anglesOut);
    
    doubledvector receiveAngles();
    dvector receiveAngles(size_t index);
    
    /**
     * Gets the current position from GPS ignoring the Y axis
     * In our Webots simulations the Y axis is the height
     * @return
     */
    transforms::Vector_3 _get_gps();
    
    /**
     * Get the current servo angle value normalizing it between [-1;1]
     * @return Servo angle value normalized between [-1;1]
     */
    double _get_motor_position(size_t index);
    
    /**
     * Sets the servo angle normalizing it between [-pi/2;pi/2]
     * @param Servo angle value normalized between [-1;1]
     */
    void _set_motor_position(size_t index, double value);
    
    /**
     * Queries the algorithm
     * @param Servos angle values at the current time step
     * @return Servos angle values for the next time step
     */
    std::vector<std::vector<double> > _compute_angles(const std::vector<std::vector<double> > &);
    
    /**
     * Calculates the controller fitness
     * @param
     * @param
     * @return
     */
    std::pair<double, std::string> _compute_fitness(double, const transforms::Vector_3 &);
    
    
    bool allConnectorsOK();
    
    void enableAllConnectors();
    
    void disableAllConnectors();
    
    void storeMatureLifeFitnessIntoFile(double fitness);
    
    void storeRebuild(std::string message);
    
    void storeFertilityOnFile();
    
    void logNumberOfReceivedGenomes();
    
    void storeProblem(std::string message, std::string phase);
    
    void storeMatingLocation(std::string partner, std::string location);
    
    void storeReproductionLocation(std::string partner, std::string location);
    
    double getRealFitness(double fitness);
    
    void logGPS();
    
    // checks whether the module is root or not
    bool isRoot();
    
    // before starting
    bool checkLocks();
    
    bool checkFallenInside();
    
    void life();
    
    // deactivate
    void death();
    
    void askToBeBuiltAgain();
    
    void readMateMessage(std::string message, id_t * mateId, double * mateFitness, std::string * mateGenome, std::string * mateMind);
    
    void updateOrganismsToMateWithList(id_t mateId, double mateFitness, std::string mateGenome, std::string mateMind);
    
    id_t selectMate();
    
    int searchForOrganism(id_t organismId);
    
    void sendAdultAnnouncement();
    
    void sendFertileAnnouncement();
    
    void sendFitnessUpdateToEvolver(double fitness);
};

#endif	/* ROOMBOT_CONTROLLER_H */