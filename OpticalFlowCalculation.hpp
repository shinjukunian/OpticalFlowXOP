//
//  OpticalFlowCalculation.hpp
//  OpticalFlow-64
//
//  Created by Morten Bertz on 2022/07/22.
//

#ifndef OpticalFlowCalculation_hpp
#define OpticalFlowCalculation_hpp

#include <stdio.h>
#include "XOPStandardHeaders.h"// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h

enum OpticalFlowMethod {
    
    OpticalFlowMethodFarneback,
    OpticalFlowMethodDIS,
    
};

typedef struct ParamItem{
    enum { is_int, is_double } type;
    union {
        int ival;
        double dval;
    } val;
}ParamItem;

typedef struct Params{
    int count;
    ParamItem *values;
    OpticalFlowMethod method;
}Params;

int calculateOpticalFlow(waveHndl inWave, Params *params, bool usePixel);


#endif /* OpticalFlowCalculation_hpp */
