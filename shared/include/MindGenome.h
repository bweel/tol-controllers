//
//  MindGenome.h
//  shared
//
//  Created by Berend Weel on 19/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#ifndef shared_MindGenome_h
#define shared_MindGenome_h

#include <string>
#include <boost/shared_ptr.hpp>

/**
 *
 * Abstract class that defines the
 *
 */
class MindGenome {
public:
    virtual ~MindGenome() {
        
    }
    
    virtual void crossoverAndMutate(boost::shared_ptr<MindGenome> genome) = 0;
    virtual void mutate() = 0;
    virtual void readString(std::string genome) = 0;
    virtual std::string toString() const = 0;
    virtual MindGenome *clone() const = 0;
};

/**
 * new_clone implementation for the use of ptr_vectors
 * with an abstract base class
 */
MindGenome *new_clone(MindGenome const& other);

#endif
