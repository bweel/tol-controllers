//
//  MindGenome.cpp
//  shared
//
//  Created by Berend Weel on 19/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "MindGenome.h"

/**
 * new_clone implementation for the use of ptr_vectors
 * with an abstract base class
 */
MindGenome *new_clone(MindGenome const& other) {
    return other.clone();
}
