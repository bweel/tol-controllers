//
//  MindGenomeManager.h
//  shared
//
//  Created by Berend Weel on 19/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef shared_MindGenomeManager_h
#define shared_MindGenomeManager_h

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

#include "MindGenome.h"

class MindGenomeManager {
public:
    virtual boost::shared_ptr<MindGenome> getGenomeFromStream(std::istream& stream) = 0;
    
    virtual boost::ptr_vector<MindGenome> readStringToArray(std::string genomeArray) = 0;
    virtual std::string genomeArrayToString(boost::ptr_vector<MindGenome> genomes) = 0;
    
    virtual boost::shared_ptr<MindGenome> createGenome(const std::vector<boost::shared_ptr<MindGenome> > & parentsGenomes) = 0;
};
    
#endif
