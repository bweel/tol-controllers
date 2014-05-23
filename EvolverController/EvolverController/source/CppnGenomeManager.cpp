#include "CppnGenomeManager.h"


CppnGenomeManager::CppnGenomeManager(time_t seed)
{
    NEAT::Globals::init();
    
    //Interesting NEAT features
    NEAT::Globals::getSingleton()->setParameterValue("MutateAddNodeProbability", 0.03);
    NEAT::Globals::getSingleton()->setParameterValue("MutateAddLinkProbability", 0.3);
    NEAT::Globals::getSingleton()->setParameterValue("MutateDemolishLinkProbability", 0.00);
    NEAT::Globals::getSingleton()->setParameterValue("MutateLinkWeightsProbability", 0.8);
    NEAT::Globals::getSingleton()->setParameterValue("MutateLinkProbability", 0.1);
    NEAT::Globals::getSingleton()->setParameterValue("MutationPower", 2.5); //Mutation power for modifying link weights
    
    //Random seed
    NEAT::Globals::getSingleton()->setParameterValue("RandomSeed", seed); //The seed for the entire experiment
    
    //I do not want to use these features now.
    NEAT::Globals::getSingleton()->setParameterValue("AddBiasToHiddenNodes", 0.0); //Adds a separate bias to all hidden nodes.
    NEAT::Globals::getSingleton()->setParameterValue("AdultLinkAge", 0.0); //If set to non-zero, non-adult links will mutate every generation. This is done to quickly optimize the link.
    NEAT::Globals::getSingleton()->setParameterValue("LinkGeneMinimumWeightForPhentoype", 0.0); //If the weight of a link is below this threshold the entire link will be ignored
    NEAT::Globals::getSingleton()->setParameterValue("AllowAddNodeToRecurrentConnection", 0.0);
    NEAT::Globals::getSingleton()->setParameterValue("AllowRecurrentConnections", 0.0);
    NEAT::Globals::getSingleton()->setParameterValue("AllowSelfRecurrentConnections", 0.0);
    NEAT::Globals::getSingleton()->setParameterValue("OnlyGaussianHiddenNodes", 0.0);
    
    //Neural network settings. These should be oke.
    NEAT::Globals::getSingleton()->setParameterValue("ExtraActivationFunctions", 1.0); //The chance that that a random activation function is used when adding a node. While larger then 1 this will always happen. When 0 all nodes will have a sigmoid activation function.
    NEAT::Globals::getSingleton()->setParameterValue("SignedActivation", 1.0); //If the generated neural network uses singed activations
    NEAT::Globals::getSingleton()->setParameterValue("ExtraActivationUpdates", 1.0); //The number of times the network gets updated before the first run. Should only matter in recurrent networks.
}


CppnGenomeManager::~CppnGenomeManager()
{
    //nix
}


CppnGenome CppnGenomeManager::createGenome(const std::vector<CppnGenome>& parentsGenomes)
{
    CppnGenome newGenome = CppnGenome(CPPN_GRID_STARTING_SIZE);
    
    switch(parentsGenomes.size()){
            
        case TWO_PARENTS:
        {
            //This egg has two genomes. Copy first, crossover and mutate with the other.
            newGenome = CppnGenome(parentsGenomes[0]);
            newGenome.crossoverAndMutate(parentsGenomes[1]);
        }break;
            
        case ONE_PARENT:
        {
            //This egg has one genome. Copy and mutate the it.
            newGenome = CppnGenome(parentsGenomes[0]);
            newGenome.mutate();
        }break;
            
        case NO_PARENTS:
        {
            //Use a new genome.
            newGenome = CppnGenome(CPPN_GRID_STARTING_SIZE);
        }break;
            
        default:
            throw CREATE_LOCATEDEXCEPTION_INFO("Unsupported number of genomes: " + TO_STR(parentsGenomes.size()));
    }
    return newGenome;
}


std::string CppnGenomeManager::genomeToString(CppnGenome genome) const
{
    return genome.toString();
}

CppnGenome CppnGenomeManager::getGenomeFromStream(std::istream& stream)
{
    CppnGenome genome = CppnGenome(stream);
    return genome;
}