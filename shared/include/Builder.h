#ifndef BirthClinicController_Builder_h
#define BirthClinicController_Builder_h

#include "BuildPlan.h"
#include "RoombotBuildPlan.h"
#include "CppnGenome.h"
#include "ActivationValueMatrix.h"


class Builder
{
    public:
    
    Builder();
    
    ~Builder();
    
    CppnGenome getGenomeFromStream(std::istream& stream);
    
    std::auto_ptr<BuildPlan> translateGenome(CppnGenome genome) const;
    
};

#endif
