#ifndef ROOMBOT_CONTROLLER_H
#define	ROOMBOT_CONTROLLER_H

//#define DEBUG_CONTROLLER
//#define DEBUG_TIMING


#include "Defines.h"
#include "EVAlgorithm.h"

#include "Vector_3.h"
#include <webots/Robot.hpp>
#include <vector>
#include <fstream>

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
    
    enum Algorithm_Types    // type of learning algorithm
    {
        A_UNDEFINED = 0,    // not defined
        A_NEAT = 1,         // HyperNEAT
        A_POWER = 2,        // RL PoWER
        A_CPG = 3,          // ???
        A_SPLINENEAT = 4    // SplineNEAT
    };
    
    // PUBLIC METHODS
    
    RoombotController();             // contructor
    
    virtual ~RoombotController();    // deconstructor
    
    void run();                 // robot behavior
    
    
    
private:
    
    // ATTRIBUTES
    
    std::string simulationDateAndTime;
    
    boost::property_tree::ptree * _parameters;  // parameters tree
    
    std::string _name;                          // whole robot's name
    int organismId;
    
    std::string genome;
    
    std::string logDirectory;                   // directory of the log file
    
    unsigned int _seed;
    
    std::size_t _r_index;                       // index of the first module
    std::size_t _r_index_root;                  // index of the root module
    std::size_t organismSize;                   // number of modules
    
    std::size_t _m_index;                       // module's index
    int _m_type;                                // module's type (0=static, 1=slave, 2=master)
    
    double _time_step;                          // time - step
    double _time_start;                         // time - start
    double _time_offset;                        // time - offset
    double _time_end;                           // time - end
    
    int _ev_type;                               // algorithm (1=HyperNEAT, 2=RL PoWER)
    std::size_t _ev_step;                       // current evaluation step
    std::size_t _ev_steps_recovery;             // time steps without fitness
    std::size_t _ev_steps_total_infancy;        // time steps to wait before evaluation during infancy
    unsigned int totalEvaluations;              // total number of evaluations
    unsigned int generation;                    // current generation
    double _ev_angular_velocity;                // speed of the motors
    
    unsigned int infancy_generations;           // number of generetion for infancy
    
    transforms::Vector_3 _position_start;       // list of transformations
    transforms::Vector_3 _position_end;         // from start to end
    
    unsigned int numMotors;                     // number of motors
    double motorRange;                          // range of motors
    
    EVAlgorithm * _algorithm;                   // learning algorithm
    
    GPS * _gps;                                 // GPS
    Emitter * _emitter;                         // emitter
    Receiver * _receiver;                       // receiver
    Receiver * deathReceiver;
    
    Motor ** _motors;                           // motors
    
    std::ofstream gpsLog;                       // GPS log
    
    std::vector<Connector *> connectors = std::vector<Connector *>();
    
    
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
    
    double getRealFitness(double fitness);
    
    void logGPS();
    
    // checks whether the module is root or not
    bool isRoot();
    
    // initial learning
    void infancy();
    
    // behavior after learning and reproduction
    void matureLife();
    
    // deactivate
    void death();
};

#endif	/* ROOMBOT_CONTROLLER_H */