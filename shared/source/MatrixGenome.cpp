//
//  MatrixGenome.cpp
//  shared
//
//  Created by Berend Weel on 16/05/14.
//  Copyright (c) 2014 Berend Weel. All rights reserved.
//

#include "MatrixGenome.h"

/**
 * Constructs a random, minimal genome with a matrix of size_x by size_y.
 * The minimal genome will contain one output node, two input nodes
 * and a bias node. The network will be fully connected and all
 * nodes, except the bias node, will have a random activation function.
 *
 * @param size_x The x size of this genome.
 * @param size_y The y size of this genome.
 */
MatrixGenome::MatrixGenome(int size_x, int size_y) {
    x_size = size_x;
    y_size = size_y;
    
    genes = std::vector<std::vector<double> >(x_size,std::vector<double>(size_y,0));
    
    random = new utils::Random();
}

/**
 * Constructs a copy of genome1.
 * The genome will not be mutated.
 *
 * @param genome1 The genome to be copied.
 */
MatrixGenome::MatrixGenome(const MatrixGenome& genome1) {
    x_size = genome1.x_size;
    y_size = genome1.y_size;
    
    genes = genome1.genes;
}

/**
 * Constructs the genome described by stream.
 * This constructor is able to process streams based on the output of the toString() function,
 * creating the exact same genome that returned the string.
 *
 * @param stream Stream containing the description of a CppnGenome.
 */
MatrixGenome::MatrixGenome(std::istream& stream) {
    std::string genomeStr;
    std::string valuesStr;
    
    stream >> genomeStr >> x_size >> y_size >> valuesStr;
    
    genes.resize(x_size);
    for(int i=0;i<x_size;i++) {
        genes[i].resize(y_size);
        for(int j=0;j<y_size;j++){
            stream >> genes[i][j];
        }
    }
}

/**
 * Destroys the CppnGenome.
 * Does not actually perform any additional clean-up.
 */
MatrixGenome::~MatrixGenome() {
    
}

/**
 * Returns a string of this genome that contains enough information to create
 * an exact copy of this genome.
 * Used in combination with the stream based constructor to write the genome to file,
 * and then reconstruct for detailed analysis.
 * Useful for debugging, logging or saving genomes for later use.
 *
 * @return Returns a string containing all information to reconstruct the genome.
 */
std::string MatrixGenome::toString() const {
    std::string result = std::string("genome ");
    result.append(TO_STR(x_size));
    result.append(" ");
    result.append(TO_STR(y_size));
    result.append(" values ");
    
    std::ostringstream oss;
    for(int i=0;i<x_size;i++) {
        for(int j=0;j<y_size;j++){
            oss << genes[i][j] << " ";
        }
    }
    result.append(oss.str());
    
    return result;
}

/**
 * Mutates this genome.
 *
 * Mutation of happens with a chance of SIZE_MUTATION_RATE
 * by adding a random numbers drawn from a normal distribution with a mean of zero
 * and a standard deviation of SIZE_MUTATION_STRENGTH
 */
void MatrixGenome::mutate() {
    for (int i=0;i<x_size;i++){
        for(int j=0;j<y_size;j++){
            if (random->uniform_real() < MATRIX_MUTATION_RATE)
            {
                genes[i][j] += random->normal_real() * MATRIX_MUTATION_STRENGTH;
            }
        }
    }
}

/**
 * Combines this genome with the input genome and then mutates this genome.
 *
 * Mutation of happens with a chance of SIZE_MUTATION_RATE
 * by adding random numbers drawn from a normal distribution with a mean of zero
 * and a standard deviation of SIZE_MUTATION_STRENGTH
 *
 * @param genome The genome to use in the crossover.
 */
void MatrixGenome::crossoverAndMutate(const MatrixGenome& genome) {
    // Uniform crossover
    for (int i=0;i<x_size;i++){
        for(int j=0;j<y_size;j++){
            if(random->uniform_real() < 0.5){
                genes[i][j] = genome.genes[i][j];
            }
        }
    }
    mutate();
}

/**
 * Returns a copy of the matrix
 *
 * @return a copy of the matrix
 */
std::vector<std::vector<double> > MatrixGenome::getMatrix() {
    return genes;
}

/**
 * Returns the x size of the matrix,
 *
 * @return Returns the size of the Activation Value Matrix.
 */
int MatrixGenome::getSizeX() const {
    return x_size;
}

/**
 * Returns the y size of the matrix
 *
 * @return Returns the size of the Activation Value Matrix.
 */
int MatrixGenome::getSizeY() const {
    return y_size;
}