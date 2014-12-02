//
//  main.cpp
//  UnitTests
//
//  Created by Berend Weel on 16/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include <iostream>
#include <boost/ptr_container/ptr_vector.hpp>
#include <random>
#include <map>
#include "NEAT.h"
#include "gtest/gtest.h"
#include "MatrixGenome.h"
#include "MatrixGenomeManager.h"
#include "Logger.h"


/*********************************************************/
/* Some tests                                            */
/*********************************************************/
TEST(MatrixGenomeStringTest, TwoByTwoMatrix) {
    MatrixGenome matrix;
    
    std::vector<std::vector<double> > values = std::vector<std::vector<double> >(2,std::vector<double>(2,0));
    matrix.setValues(values);
    
    EXPECT_EQ("MATRIX 2 2 VALUES 0 0 0 0 ", matrix.toString());
}

TEST(MatrixGenomeStringTest, EmptyMatrix) {
    MatrixGenome matrix;
    
    EXPECT_EQ("MATRIX 0 0 VALUES ", matrix.toString());
}

TEST(MatrixGenomeStringTest, EmptyMatrixLoad) {
    MatrixGenome matrix;

    std::istringstream iss(matrix.toString());
    
    MatrixGenome matrix2(iss);
    
    EXPECT_EQ(matrix.toString(), matrix2.toString());
}

TEST(MatrixGenomeStringTest, TwoByTwoMatrixRead) {
    MatrixGenome matrix(2, 2);
    
    std::istringstream iss(matrix.toString());
    
    MatrixGenome matrix2(iss);
    
    EXPECT_EQ(matrix.toString(),matrix2.toString());
}

TEST(MatrixGenomeStringTest, FourByTwoMatrixRead) {
    MatrixGenome matrix(4, 2);
    
    std::istringstream iss(matrix.toString());
    
    MatrixGenome matrix2(iss);
    
    EXPECT_EQ(matrix.toString(),matrix2.toString());
}

TEST(MatrixGenomeStringTest, TwoByFourMatrixRead) {
    MatrixGenome matrix(2, 4);
    
    std::istringstream iss(matrix.toString());
    
    MatrixGenome matrix2(iss);
    
    EXPECT_EQ(matrix.toString(),matrix2.toString());
}


TEST(MatrixGenomeStringTest, MatriXMutate) {
    MatrixGenome matrix(2, 2);
    
    std::string before = matrix.toString();
    
    matrix.mutate();
        
    EXPECT_NE(before, matrix.toString());
}

TEST(MatrixGenomeManager, ReadString) {
    MatrixGenomeManager manager;
    std::string genome = "MATRIX 18 2 VALUES 0.48421 0.625354 1.77726 0.529473 0.589739 0.398873 0.666612 0.804271 0.425856 0.440141 0.358484 0.534888 0.386149 0.515436 -0.529771 0.689018 0.510393 0.303181 1.24373 0.600268 1.91244 -0.452122 -0.119038 1.25886 -0.229843 1.5897 0.0137829 2.15796 0.46511 -0.722367 -0.881665 0.619514 0.774269 0.510847 -1.32993 0.556231  | MATRIX 18 2 VALUES 0 0 0 0 0 0 0 0 0 0 0 0 0 0 -0 0 0 0 0 0 0 -0 -0 0 -0 0 0 0 0 -0 -0 0 0 0 -0 0";
    
    boost::ptr_vector<MindGenome> genomes = manager.readStringToArray(genome);
    
    ASSERT_EQ(genomes.size(),2);
}

TEST(RandomGeneration, BinomialDistribution) {
    NEAT::Globals::init();
    NEAT::Random random = NEAT::Globals::getSingleton()->getRandom();
    std::map<int, int> hist;
    for (int n = 0; n < 1000000; ++n) {
        ++hist[random.getRandomWithinRange(-1, 1)];
    }
    for (auto p : hist) {
        std::cout << setw(3) << p.first << ' '
        << std::string(p.second/10000, '*') << " (" << p.second << ")" << '\n';
    }   
}

TEST(Logging, RootLogger){
    log4cpp::Category &root = Logger::getRoot();

    root.error("Logging an Error");
    root.warn("Logging a Warning");
    
    Logger::shutdown();
}

TEST(Logging, FileLogger){
    log4cpp::Category &a1 = Logger::getInstance(std::string("sub1"));

    a1.error("Logging an Error");
    a1.warn("Logging a Warning");
    
    Logger::shutdown();
}

int main(int argc, const char * argv[])
{
    testing::InitGoogleTest(&argc, (char**)argv);
    return RUN_ALL_TESTS();
}

