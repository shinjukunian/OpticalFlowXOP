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

struct FarnebackParams {
    double pyr_scale;
    int levels;
    int winSize;
    int iterations;
    int poly_n;
    double poly_sigma;
    int flags;
};


int makeOutputWaves(waveHndl inWave, waveHndl *outX, const char *xName, waveHndl *outY, const char *yName);
//int calculateOpticalFlow_Farneback(waveHndl inWave, waveHndl outX, waveHndl outY, FarnebackParams params);

#endif /* Helper_hpp */
