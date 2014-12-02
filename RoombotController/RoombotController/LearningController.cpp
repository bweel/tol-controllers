//
//  LearningController.cpp
//  RoombotController
//
//  Created by Berend Weel on 21/11/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "LearningController.h"
#include "Random.h"
#include "HyperNEAT.h"
#include "RL_PoWER.h"
#include "SplineNeat.h"
#include "MatrixGenomeManager.h"
#include "RoombotController.h"

const int LearningController::GPS_LOG_PERIOD = 50;

LearningController::LearningController(WorldModel &wm) :
    worldModel(wm)
{
    
}

LearningController::~LearningController() {
}

void LearningController::initialise() {
    // Learning
    std::string learningAlgorithmType = worldModel.parameters.get<std::string>("Algorithm.Type");
    unsigned int infancyEvaluations = worldModel.parameters.get<unsigned int>("Algorithm.Evaluations");
    unsigned int infancyDuration = worldModel.parameters.get<unsigned int>("Algorithm.Infancy_Duration");
    
    double singleEvaluationTime = infancyDuration / infancyEvaluations;
    double timeStep = worldModel.TIME_STEP / 1000.0;
    evaluationSteps = singleEvaluationTime / timeStep;
    recoverySteps = 0;
    evaluationSteps = evaluationSteps - recoverySteps;
    unsigned int totalEvaluations = worldModel.timeToLive / singleEvaluationTime;
    
    motorAngularVelocity = worldModel.parameters.get<double>("Algorithm.Angular_Velocity");
    
    double seed = Utils::Random::getInstance()->uniform_real();
    std::string logDirectory;
    if(learningAlgorithmType == "HyperNEAT") {
        logDirectory = initialiseLogDirectory(worldModel.projectPath, worldModel.robotName, "HyperNEAT", worldModel.logDirectory);
        learningAlgorithm = std::unique_ptr<LearningAlgorithm>(new HyperNEAT(
                                           seed,
                                           worldModel.parametersPath,
                                           worldModel.logDirectory,
                                           initialiseShapeSize(worldModel.parameters.get_child("Shape")),
                                           initialiseModuleMapping(worldModel.robotIndex, worldModel.organismSize, worldModel.robotName, worldModel.parameters.get_child("Robot")),
                                           worldModel.numMotors,
                                           1));
    } else if(learningAlgorithmType == "RLPower") {
        logDirectory = initialiseLogDirectory(worldModel.projectPath, worldModel.robotName, "RL_PoWER", worldModel.logDirectory);
        learningAlgorithm = std::unique_ptr<LearningAlgorithm>(new RL_PoWER(
                                          seed,
                                          "",
                                          logDirectory,
                                          worldModel.TIME_STEP * 1e-3,    // Webots time step converted from ms to s
                                          motorAngularVelocity,
                                          totalEvaluations,
                                          worldModel.organismSize * worldModel.numMotors
                                          ));
    } else if (learningAlgorithmType == "SplineNEAT") {
            logDirectory = initialiseLogDirectory(worldModel.projectPath, worldModel.robotName, "SplineNEAT", worldModel.logDirectory);
            learningAlgorithm = std::unique_ptr<LearningAlgorithm>(new SplineNeat(
                                        seed,
                                        worldModel.parametersPath,
                                        logDirectory,
                                        initialiseModuleMapping(worldModel.robotIndex, worldModel.organismSize, worldModel.robotName, worldModel.parameters.get_child("Robot")),
                                        worldModel.numMotors,
                                        worldModel.TIME_STEP * 1e-3,  // Webots time step converted from ms to s
                                        motorAngularVelocity,
                                        worldModel.organismSize));
    }else {
            throw std::runtime_error("Algorithm Unknown");
    }
    
    fitnessMeasure = FitnessMeasure::getFitnessMeasure(learningAlgorithmType, worldModel);
    
    // set variables for writing on files
    boost::filesystem::path dirpath(logDirectory);
    std::cout << "Opening fitness log file: " << dirpath / "fitness.log" << std::endl;
    fitnessLog = std::ofstream((dirpath / "fitness.log").c_str(), std::ofstream::app);
    
    std::cout << "Opening location log file: " << dirpath / "fitness.log" << std::endl;
    gpsLog = std::ofstream((dirpath / "location.log").c_str(), std::ofstream::app);
    
    fitnessLog << "#Generation Evaluation Fitness 2eDisSq Distance Speed RLFitness  (Seed: " << seed << ")" << std::endl;
    gpsLog << "#Evaluation x y z" << std::endl;
    
    // set mind
    boost::ptr_vector<MindGenome> genome;
    MatrixGenomeManager manager;
    if(worldModel.mindGenome.length() == 0){
        genome = learningAlgorithm->getRandomInitialMinds();
        worldModel.mindGenome = manager.genomeArrayToString(genome);
    }else{
        genome = manager.readStringToArray(worldModel.mindGenome);
    }
    learningAlgorithm->setInitialMinds(genome,worldModel.numMotors,worldModel.organismSize);
}

void LearningController::finalise() {
    if(learningAlgorithm){
        learningAlgorithm->save();
    }
}


// compute new angles for each motors of each module of the organisms
std::vector<std::vector<double> > LearningController::computeAngles(const std::vector<std::vector<double> > & angles)
{
    size_t size = angles.size();
    std::vector<std::vector<double> > result(size,std::vector<double>(worldModel.numMotors,0.0));
    
    learningAlgorithm->reset();
    
    for (std::size_t index = 0; index < size; index++) {
        for(size_t j=0;j<worldModel.numMotors;j++){
            learningAlgorithm->setParameter((index*worldModel.numMotors)+j, angles[index][j]);
        }
    }
    
    learningAlgorithm->setParameter(worldModel.organismSize*worldModel.numMotors, std::sin(motorAngularVelocity * (worldModel.now - worldModel.lifetimeStart)));
    
    learningAlgorithm->update();
    
    for (std::size_t index = 0; index < size; index++) {
        for(size_t j=0;j<worldModel.numMotors;j++){
            result[index][j] = learningAlgorithm->getParameter((index*worldModel.numMotors)+j);
        }
    }
    
    return result;
}

void LearningController::step() {
    // if it is not time for evaluation yet
    if (evaluationStep < evaluationSteps)
    {
        evaluationStep += 1;  // increase current
        
        // if the recovery time ends
        if ((evaluationStep - 1) == recoverySteps) {
            fitnessMeasure->markStart();
        }
        
        // GPS logging
        if(evaluationStep % GPS_LOG_PERIOD == 0){
            logGPS();
        }
    }
    
    // if it is time for evaluation
    else
    {
        fitnessMeasure->markEnd();
        double fitness = fitnessMeasure->computeFitness(); // computeFitness((endTime - startTime), (endPosition - startPosition));   // compute fitness
        learningAlgorithm->setEvaluationFitness(fitness);
//        learningAlgorithm->setEvaluationFitnessAlt(fitness.second);
        
        fitnessLog << learningAlgorithm->getGeneration() << " " << learningAlgorithm->getEvaluation() << " " << fitness << std::endl;
        
        if (!learningAlgorithm->nextEvaluation()) {
            learningAlgorithm->save();
        }
        
        fitnessMeasure->markStart();
        evaluationStep = 0;
    }
}

// initialize directory
std::string LearningController::initialiseLogDirectory(const std::string & directory, const std::string & name, const std::string & algorithm, const std::string &logdir)
{
    boost::filesystem::path path(directory);
    
    path /= RoombotController::RESULTS_DIR;
    
    if(worldModel.simulationDateAndTime == "random"){
        std::time_t rawtime;
        std::tm* timeinfo;
        char buffer [80];
        std::time(&rawtime);
        timeinfo = std::localtime(&rawtime);
        std::strftime(buffer,80,"%Y-%m-%d-%H.%M.%S",timeinfo);
        
        worldModel.simulationDateAndTime = buffer;
    }
    
    path /= worldModel.simulationDateAndTime;
    
    path /= name;
    
    path /= algorithm;
    
    if(logdir.length() > 0){
        path /= logdir;
    }
    
    //    path /= boost::lexical_cast<std::string>(instance);
    
    if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path)) {
        boost::filesystem::remove_all(path);
        //throw std::runtime_error("Directory Already Existing");
    }
    
    boost::filesystem::create_directories(path);
    
    return path.string();
}

// initialize shape size
Vector3<double> LearningController::initialiseShapeSize(const boost::property_tree::ptree & root)
{
    Vector3<double> temp(root.get<double>("X"),
                         root.get<double>("Y"),
                         root.get<double>("Z"));
    
    return temp;
}

// initialize module mapping
std::vector<Vector3<double> > LearningController::initialiseModuleMapping(std::size_t offset, std::size_t size, const std::string & name, const boost::property_tree::ptree & root)
{
    boost::property_tree::ptree parent = root, child;
    
    std::vector<Vector3<double> > result(size);
    
    for (std::size_t index = 0; index < size; index++) {
        /* Get Module Property Tree */
        child = parent.get_child(name + ":" + boost::lexical_cast<std::string>(index + offset));
        
        /* Get Module 3D Index */
        ptree index3D = child.get_child("Index_3D");
        Vector3<double> p_index(index3D.get<double>("X"),index3D.get<double>("Y"),index3D.get<double>("Z"));
        
        result[index] = p_index;
        
        //std::cout << index << " -> " << result[index] << std::endl;
    }
    return result;
}

// logging for GPS
void LearningController::logGPS() {
    gpsLog << learningAlgorithm->getEvaluation() << " " << worldModel.position.x << " 0.0 " << worldModel.position.z << std::endl;
}
