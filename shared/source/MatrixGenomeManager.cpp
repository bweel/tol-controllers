//
//  MatrixGenomeManager.cpp
//  EvolverController
//
//  Created by Berend Weel on 16/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "MatrixGenomeManager.h"
#include "JGTL_LocatedException.h"


MatrixGenomeManager::MatrixGenomeManager(time_t seed)
{
}


MatrixGenomeManager::~MatrixGenomeManager()
{
    //nix
}


boost::shared_ptr<MindGenome> MatrixGenomeManager::createGenome(const std::vector<boost::shared_ptr<MindGenome> > & parentsGenomes)
{
    // empty genome
    boost::shared_ptr<MindGenome> newGenome;
    
    switch(parentsGenomes.size()){
            
        case TWO_PARENTS:
        {
            //This egg has two genomes. Copy first, crossover and mutate with the other.
            boost::shared_ptr<MatrixGenome> parent0 = boost::dynamic_pointer_cast<MatrixGenome>(parentsGenomes[0]);
            newGenome = boost::shared_ptr<MindGenome>(new MatrixGenome(*parent0));
            newGenome->crossoverAndMutate(parentsGenomes[1]);
        }break;
            
        case ONE_PARENT:
        {
            //This egg has one genome. Copy and mutate the it.
            boost::shared_ptr<MatrixGenome> parent0 = boost::dynamic_pointer_cast<MatrixGenome>(parentsGenomes[0]);
            newGenome = boost::shared_ptr<MindGenome>(new MatrixGenome(*parent0));
            newGenome->mutate();
        }break;
            
        case NO_PARENTS:
        {
            // use an empty genome
        }break;
            
        default:
            throw CREATE_LOCATEDEXCEPTION_INFO("Unsupported number of genomes: " + TO_STR(parentsGenomes.size()));
    }
    return newGenome;
}


std::string MatrixGenomeManager::genomeToString(MatrixGenome genome) const
{
    return genome.toString();
}

boost::shared_ptr<MindGenome> MatrixGenomeManager::getGenomeFromStream(std::istream& stream)
{
    boost::shared_ptr<MindGenome> genome (new MatrixGenome(stream));
    return genome;
}

boost::ptr_vector<MindGenome> MatrixGenomeManager::readStringToArray(std::string genomeArray){
    boost::ptr_vector<MindGenome> genomes;

    std::string genomeString = genomeArray;
    
    std::string delimiter = " | ";
    std::string matrix = genomeString.substr(0,genomeString.find(delimiter));
    while(matrix.length() > 0){
        std::size_t pos = genomeString.find(delimiter);
        if(pos != std::string::npos){
            genomeString = genomeString.substr(pos+delimiter.length(),std::string::npos);
        }else{
            genomeString = "";
        }
        
        std::stringstream mstream(matrix);
        
        genomes.push_back(new MatrixGenome(mstream));
        matrix = genomeString.substr(0, genomeString.find(delimiter));
    }
    return genomes;
}

std::string MatrixGenomeManager::genomeArrayToString(boost::ptr_vector<MindGenome> genomes) {
    std::stringstream out;
    
    for (int i=0;i< genomes.size();i++){
        out << genomes[i].toString();
        if(i<genomes.size()-1){
            out << " | ";
        }
    }
    
    return out.str();
}