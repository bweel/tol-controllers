#ifndef OM_DEFINES_H
#define OM_DEFINES_H
//All constants used in one or more Organism Manager (OM) files are defined here
#include <cmath>
//#include <boost/lexical_cast.hpp>
#include <sstream>
#include <sys/types.h>
#include <vector>

/************************/
/*** Log4cpp settings ***/
/************************/
//Mainly keywords that are used inside the log file to discriminate between different logs
#define LOG_CATEGORY "log"		//The name of the log category
#define CONVERSION_PATTERN "%m%n"	//The conversion pattern used by the logger

#define NUMBER_OF_ORGANISMS_LOG "nrOfOrgs"
#define NUMBER_OF_MODULES_LOG "nrOfMods"
#define EGG_POSITION_LOG "eggPos"
#define ORGANISM_POSITION_LOG "orgPos"
#define HATCH_LOG "hatch"
#define HATCH_FAIL_TIMEOUT_LOG "hatchTimeOut"
#define FERTILIZATION_LOG "fertil"
#define DEATH_LOG "death"
#define START_LOG "logStart"
#define TIME_LOG "time"
#define KILL_LOG "killed"

#define ARENA_SIZE_STR "ArenaSize"
#define FERTILIZATION_RANGE_STR "FertilizationRange"
#define TIME_TO_LIVE_STR "TimeToLive"

#define SUCCES "succes"
#define NO_ORGANISM "noOrg"
#define NO_EGG "noEgg"
#define NO_ORGANISM_AND_EGG "noOrgEgg"
#define EGG_FULL "eggFull"

#define HATCH_NOT_ENOUGH_MODULES "modFail"
#define HATCH_EMPTY_ORGANISM "emptFail"
#define HATCH_POSITION_UNAVAILABLE "posFail"
#define HATCH_UNDEFINED "undef"

/************************/
/*** Module  settings ***/
/************************/
#define CURRENT_MODULE RoombotModule(root, i, mainEmitter, arenaSize)
#define GROUND_HEIGHT 0.055	//The height a module has to be placed 'above zero' to be placed ON the ground as opposed to IN the ground
#define MODULE_WIDTH 0.11	//The width of a module, the module is assumed to be two width long
#define COLLISION_THRESHOLD 0.11 //Distance needed between two modules to make sure they can not collide.
#define SPARE_DISTANCE 5	//Distance the spare parts have with respect to the side of the arena
#define SERVO_THRESHOLD 0.03 //If the difference between the servo rotation and 0 is less then this value, the servo is considered to be 'reset'.

//Connector positions

//             Joost                              Berend
//
//               N                                 W2Y
//          +---------+                        +---------+
//          |white /  |                        |white /  |
//          |     /   |                        |     /   |
//       NW | NT / NB | NE                 W2X | W2Z/B2Z | B2X
//          |   /     |                        |   /     |
//          |  / black|                        |  / black|
//          +---------+                        +---------+
//          +---------+                        +---------+
//          |white /  |                        |white /  |
//          |     /   |                        |     /   |
//       SW | ST / SB | SE                 W1X | W1Z/B1Z | B1X
//          |   /     |                        |   /     |
//          |  / black|                        |  / black|
//          +---------+                        +---------+
//               S                                 B1Y

#define SOUTH_CONNECTOR 0
#define SOUTH_BOT_CONNECTOR 1
#define SOUTH_EAST_CONNECTOR 2
#define SOUTH_WEST_CONNECTOR 3
#define SOUTH_TOP_CONNECTOR 4
#define NORTH_BOT_CONNECTOR 5
#define NORTH_EAST_CONNECTOR 6
#define NORTH_CONNECTOR 7
#define NORTH_WEST_CONNECTOR 8
#define NORTH_TOP_CONNECTOR 9
#define NR_OF_CONNECTORS 10
static const std::vector<std::string> CONNECTOR_NAMES =
{"CB1Y",
"CB1Z",
"CB1X",
"CW1X",
"CW1Z",
"CB2Z",
"CB2X",
"CW2Y",
"CW2X",
"CW2Z"};

/************************/
/* Supervisor  settings */
/************************/
#define SUPERVISOR_ID 0                 //The sending ID used by the Organism Manager
#define ROOMBOT_EMITTER_NAME "Emitter"		//The name of the main emitter of the supervisor
#define ROOMBOT_RECEIVER_NAME "Receiver"	//The name of the main receiver of the supervisor
#define ROOMBOT_DEATH_RECEIVER_NAME "DeathReceiver"

/***************************/
/* Genome Related settings */
/***************************/
#define CURRENT_GENOME_MANAGER CppnGenomeManager(seed)
#define NR_OF_GENOMES 2	//Maximum number of parents per egg
#define TWO_PARENTS 2	//The integer number for two parents
#define ONE_PARENT 1	//The integer number for one parent
#define NO_PARENTS 0	//The integer number for no parents

//Input values for NEAT_GeneticNodeGene
#define TYPE_INPUT "NetworkSensor"
#define TYPE_OUTPUT "NetworkOutputNode"
#define DRAW_POS_ZERO 0
#define DRAW_POS_HALF_MAX numeric_limits<double>::max()/2
#define RANDOM_ACTIVATION_FALSE false
#define RANDOM_ACTIVATION_TRUE true

//Input values for NEAT_GeneticIndividual
#define CREATE_TOPOLOGY_TRUE true
#define CREATE_TOPOLOGY_FALSE false
#define EDGE_DENSITY_FULLY_CONNECTED 1

//The names of the input and output nodes of the cppn network used as a genome
//The network can fill a matrix of any size with numbers by taking
//as input the x and y value of the matrix cell. The then becomes the value
//of that cell. Note that x and y values of this matrix are centred around
//zero meaning that a 5 by 5 matrix would take x and y values between -2 and 2
//instead of between 0 and 4.
#define INPUT_X "x"			//The name of the node taking the x-axis as input
#define INPUT_Y "y"			//The name of the node taking the y-axis as input
#define INPUT_BIAS "bias"	//The name of the node bias node
#define OUTPUT "output"		//The name of the output node


/***************************/
/***** Macros and Such *****/
/***************************/
template <class T> inline std::string to_string (const T& t){
	std::stringstream ss;
	ss << t;
	return ss.str();
}

//#define TO_STR(X) boost::lexical_cast<std::string>((X))
#define TO_STR(X) to_string(X)
#define RANDOM NEAT::Globals::getSingleton()->getRandom()

//Type definitions
typedef id_t GenomeID;

//Enumerators
enum HatchStatus { HATCHABLE, NOT_ENOUGH_MODULES, EMPTY_ORGANISM, POSITION_UNAVAILABLE };
enum KillCondition { NONE, AGE, SIZE, SMALLAGE};
enum Shape_Encoding_Type
{
    CPPN = 0x00,
    OTHER = 0x02
};









#define MODULE_DEF_BASE_NAME "MODULE_"
#define MODULE_BASE_NAME "module_"
#define ORGANISM_BASE_NAME "organism_"
#define ROOMBOT_CONTROLLER_NAME "RoombotController"
#define CONTROLLER_ARGS_PATH "../../Arguments/"
#define RESULTS_PATH "../../Results"



#define EMITTER_NAME "emitter"
#define RECEIVER_NAME "receiver"
static const int EVOLVER_CHANNEL = 1000;
static const int CLINIC_CHANNEL = 1001;
static const int MODIFIER_CHANNEL = 1002;
static const int DEATH_CHANNEL = 1003;
static const int SCREENSHOT_CHANNEL = 1004;




#define SIZE_MUTATION_RATE 0.6			//Chance the size will change
#define SIZE_MUTATION_STRENGTH 1		//The strength with which the size will change if it changes
#define THRESHOLD 0                     //Threshold that the cppn has to exceed to indicate module placement at a particular coordinate
#define CPPN_GRID_STARTING_SIZE 5		//Grid size for new cppnGenomes.





const int ARENA_SIZE = 30;
const id_t NUMBER_OF_MODULES = 30;
const int INITIAL_POPULATION = 5;
const Shape_Encoding_Type SHAPE_ENCODING = CPPN;
const double CLINIC_SAFE_DISTANCE = 5;





/// These are now in seconds!
const unsigned int MATING_TIME = 500;
const unsigned int DYING_TIME = 1000;
const unsigned int INFANCY_DURATION = 500;
const unsigned int SEND_FITNESS_TO_EVOLVER_INTERVAL = 50;
const unsigned int ROOMBOT_WAITING_TIME = 10;


#endif
