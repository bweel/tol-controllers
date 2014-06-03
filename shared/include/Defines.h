#ifndef OM_DEFINES_H
#define OM_DEFINES_H

#include <cmath>
#include <sstream>
#include <sys/types.h>
#include <vector>



/************************/
/*** Module  settings ***/
/************************/

#define GROUND_HEIGHT 0.055	//The height a module has to be placed 'above zero' to be placed ON the ground as opposed to IN the ground
#define MODULE_WIDTH 0.11	//The width of a module, the module is assumed to be two width long
#define COLLISION_THRESHOLD 0.11 //Distance needed between two modules to make sure they can not collide.
#define SPARE_DISTANCE 5	//Distance the spare parts have with respect to the side of the arena
#define SERVO_THRESHOLD 0.03 //If the difference between the servo rotation and 0 is less then this value, the servo is considered to be 'reset'.

//Connector positions (please notice that the up connectors are in white faces and the bottom ones in black faces)

//             Joost                         Berend+Emanuele
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

// list of connectors from Joost
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

// vector for the translation from Joost to Berend+Emanuele
static const std::vector<std::string> CONNECTOR_NAMES = {
    "CB1Y",
    "CB1Z",
    "CB1X",
    "CW1X",
    "CW1Z",
    "CB2Z",
    "CB2X",
    "CW2Y",
    "CW2X",
    "CW2Z"};


// Emitters and receivers names in Webots
#define ROOMBOT_EMITTER_NAME "Emitter"		//The name of the main emitter of the supervisor
#define ROOMBOT_RECEIVER_NAME "Receiver"	//The name of the main receiver of the supervisor
#define ROOMBOT_DEATH_RECEIVER_NAME "DeathReceiver"
#define ROOMBOT_GENOME_RECEIVER_NAME "GenomeReceiver"
#define ROOMBOT_GENOME_EMITTER_NAME "GenomeEmitter"






/****************************/
/*** Supervisors settings ***/
/****************************/

#define EMITTER_NAME "emitter"
#define RECEIVER_NAME "receiver"






/***************************/
/* Genome Related settings */
/***************************/

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
#define INPUT_X "x"			//The name of the node taking the x-axis as input
#define INPUT_Y "y"			//The name of the node taking the y-axis as input
#define INPUT_BIAS "bias"	//The name of the node bias node
#define OUTPUT "output"		//The name of the output node






/****************************/
/**** VARIOUS AND USEFUL ****/
/****************************/

#define TWO_PARENTS 2	//The integer number for two parents
#define ONE_PARENT 1	//The integer number for one parent
#define NO_PARENTS 0	//The integer number for no parents

#define RANDOM NEAT::Globals::getSingleton()->getRandom()






/***************************/
/***** Macros and Such *****/
/***************************/

template <class T> inline std::string to_string (const T& t){
	std::stringstream ss;
	ss << t;
	return ss.str();
}
#define TO_STR(X) to_string(X)

//Type definitions
typedef id_t GenomeID;

enum Algorithm_Types    // type of learning algorithm
{
    A_UNDEFINED = 0,    // not defined
    A_NEAT = 1,         // HyperNEAT
    A_POWER = 2,        // RL PoWER
    A_CPG = 3,          // ???
    A_SPLINENEAT = 4    // SplineNEAT
};

enum MatingType
{
    MATING_SELECTION_BY_EVOLVER = 0,
    MATING_SELECTION_BY_ORGANISMS = 1
};

enum DeathType
{
    DEATH_SELECTION_BY_EVOLVER = 0,
    DEATH_SELECTION_BY_TIME_TO_LIVE = 1
};



/*********************************/
/***** Names, paths and such *****/
/*********************************/

#define MODULE_DEF_BASE_NAME "MODULE_"
#define MODULE_BASE_NAME "module_"
#define ORGANISM_BASE_NAME "organism_"
#define ROOMBOT_CONTROLLER_NAME "RoombotController"
#define CONTROLLER_ARGS_PATH "../../Arguments/"
#define RESULTS_PATH "../../Results/"

/***************************/
/***** Terminal Colors *****/
/***************************/
//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */


#endif
