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

int makeOutputWaves(waveHndl inWave, waveHndl *outX, const char *xName, waveHndl *outY, const char *yName, bool usePixel);


#endif /* Helper_hpp */
