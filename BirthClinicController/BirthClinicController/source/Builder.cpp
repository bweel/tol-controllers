#include "Builder.h"


Builder::Builder()
{
    NEAT::Globals::init();
}


Builder::~Builder()
{
    //nix
}


CppnGenome Builder::getGenomeFromStream(std::istream& stream)
{
    CppnGenome genome = CppnGenome(stream);
    return genome;
}


std::auto_ptr<BuildPlan> Builder::translateGenome(CppnGenome genome) const
{
    ActivationValueMatrix matrix = ActivationValueMatrix(genome.getCppn(), genome.getSize(), genome.getSize());
    RoombotBuildPlan* buildPlan = new RoombotBuildPlan(genome.getSize());
    
    Coordinate max;
    Coordinate maxNeighbour;
    int x, y;
    bool horizontal;
    stack<Coordinate> neighboursChecked;
    
    //Get first module
    do{
        max = matrix.getMax();
        matrix.setUsed(max, true);
        //If we failed to get even one module, return.
        if(!max.found) return std::auto_ptr<BuildPlan> (buildPlan);
        
        maxNeighbour = matrix.getMaxNeighbour(max);
        matrix.setUsed(maxNeighbour, true);
    } while (!max.found || !maxNeighbour.found);
    
    //Add the first module to the build plan
    horizontal = (max.y == maxNeighbour.y);
    y=std::min(max.y, maxNeighbour.y);
    x=std::min(max.x, maxNeighbour.x);
    buildPlan->add(x, y, horizontal);
    matrix.addModuleCoordinate(max);
    matrix.addModuleCoordinate(maxNeighbour);
    
    //Find the rest of the modules
    while (max.found) {
        //Get the max value adjacent to the current organism
        max = matrix.getMaxAdjacent();
        matrix.setUsed(max, true);
        if(!max.found) break;
        
        //This module will dock to more then one point, do not allow it.
        if(buildPlan->numberOfNeighbours(max.x, max.y) > 1) continue;
        
        //Find smallest difference
        maxNeighbour = matrix.getMaxNeighbour(max);
        
        //Set used and add to queue
        while (maxNeighbour.found) {
            matrix.setUsed(maxNeighbour, true);
            
            //This cell docks at two points, ignore it permanently
            if(buildPlan->numberOfNeighbours(maxNeighbour.x, maxNeighbour.y) > 1){
                maxNeighbour = matrix.getMaxNeighbour(max);
                continue;
            }
            
            horizontal = (max.y == maxNeighbour.y);
            y=std::min(max.y, maxNeighbour.y);
            x=std::min(max.x, maxNeighbour.x);
            
            if(buildPlan->numberOfNeighbours(x, y, horizontal) == 1){
                buildPlan->add(x,y ,horizontal);
                matrix.addModuleCoordinate(max);
                matrix.addModuleCoordinate(maxNeighbour);
                break;
            }
            
            //This module docks to more then one point, ignore it.
            //The cell might still be usable, add it to neighbours checked.
            neighboursChecked.push(maxNeighbour);
            maxNeighbour = matrix.getMaxNeighbour(max);
        }
        
        while(!neighboursChecked.empty()){
            matrix.setUsed(neighboursChecked.top(), false);
            neighboursChecked.pop();
        }
    }
    
    return std::auto_ptr<BuildPlan> (buildPlan);
}