//
//  ParametersReader.cpp
//  shared
//
//  Created by Berend Weel on 27/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include <boost/property_tree/ptree.hpp>
#include "ParametersReader.h"

boost::property_tree::ptree * ParametersReader::parametersTree = NULL;
