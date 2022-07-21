//
//  Helper.hpp
//  OpticalFlow-64
//
//  Created by Morten Bertz on 2022/07/14.
//

#ifndef Helper_hpp
#define Helper_hpp

#include <stdio.h>
#include "XOPStandardHeaders.h"// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h

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
}Params;

int makeOutputWaves(waveHndl inWave, waveHndl *outX, const char *xName, waveHndl *outY, const char *yName, bool usePixel);

//int calculateOpticalFlow_Farneback(waveHndl *inWave, waveHndl *outX, waveHndl *outY, Params *params);

#endif /* Helper_hpp */
