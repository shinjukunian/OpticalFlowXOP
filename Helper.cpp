//
//  Helper.cpp
//  OpticalFlow-64
//
//  Created by Morten Bertz on 2022/07/14.
//


#include "Helper.hpp"


int makeOutputWaves(waveHndl inWave, waveHndl *outX, const char *xName, waveHndl *outY, const char *yName, bool usePixel){
    
    int result=0;
    
    int numDimensions;
    CountInt dimensionSizes[MAX_DIMENSIONS+1];
    
    if(result=MDGetWaveDimensions(inWave, &numDimensions, dimensionSizes)){
        return result;
    }
    
    if(numDimensions != 3){
        return kNeeds3DWave;
    }
    
    CountInt width=dimensionSizes[ROWS];
    CountInt height=dimensionSizes[COLUMNS];
    CountInt frames=dimensionSizes[LAYERS];
    
    char outWaveName[MAX_OBJ_NAME+1];
    CountInt outDimensionSizes[MAX_DIMENSIONS+1] = {0};
   
    strcpy(outWaveName, xName);
    outDimensionSizes[ROWS] = width;
    outDimensionSizes[COLUMNS] = height;
    outDimensionSizes[LAYERS] = frames -1;
    

    if(result=MDMakeWave(outX, outWaveName, NULL, outDimensionSizes, NT_FP32, 1)){
        return result;
    }

    strcpy(outWaveName, yName);
    if (result=MDMakeWave(outY, outWaveName, NULL, outDimensionSizes, NT_FP32, 1)){
        return result;
    }
    
    double deltaX;
    double x0;
    char units[MAX_UNIT_CHARS+1];
    
    for(int i=0;i<2;i+=1){
        if(result=MDGetWaveScaling(inWave, i, &deltaX, &x0)){
            return result;
        }
        if(result = MDSetWaveScaling(*outX, i, &deltaX, &x0)){
            return result;
        }
        if(result = MDSetWaveScaling(*outY, i, &deltaX, &x0)){
            return result;
        }
        if(result=MDGetWaveUnits(inWave, i, units)){
            return result;
        }
        if(result = MDSetWaveUnits(*outX, i, units)){
            return result;
        }
        if(result = MDSetWaveUnits(*outY, i, units)){
            return result;
        }
    }
    
    if(!usePixel){
        if(result=MDGetWaveUnits(inWave, 0, units) || (result = MDSetWaveUnits(*outX, -1, units))){
            return result;
        }
        
        if(result=MDGetWaveUnits(inWave, 1, units) || (result = MDSetWaveUnits(*outY, -1, units))){
            return result;
        }
    }
    
    return result;
}






/*
 Params params;
 params.count = 7;
 
 ParamItem items[7];
 params.values = items;
 params.values[0].type = ParamItem::is_double;
 params.values[1].type = ParamItem::is_int;
 params.values[2].type = ParamItem::is_int;
 params.values[3].type = ParamItem::is_int;
 params.values[4].type = ParamItem::is_int;
 params.values[5].type = ParamItem::is_double;
 params.values[6].type = ParamItem::is_int;
 
 

 if (p->PSFlagEncountered) {
     // Parameter: p->pyr_scale
     params.values[0].val.dval=p->pyr_scale;
 }
 else{
     params.values[0].val.dval=0.5;
 }

 if (p->LFlagEncountered) {
     // Parameter: p->levels
     params.values[1].val.ival=p->levels;
 }
 else{
     params.values[1].val.ival=3;
 }

 if (p->WFlagEncountered) {
     // Parameter: p->winsize
     params.values[2].val.ival=p->winsize;
 }
 else{
     params.values[2].val.ival=15;
 }

 if (p->NFlagEncountered) {
     // Parameter: p->iterations
     params.values[3].val.ival=p->iterations;
 }
 else{
     params.values[3].val.ival=3;
 }

 if (p->GFlagEncountered) {
     params.values[6].val.ival=OPTFLOW_FARNEBACK_GAUSSIAN;
 }
 
 if (p->POLYFlagEncountered) {
     params.values[4].val.ival=p->poly_n;
     params.values[5].val.dval=p->poly_sigma;
         // Parameter: p->poly_n

         // Parameter: p->poly_sigma
 }
 else{
     params.values[4].val.ival=5;
     params.values[5].val.dval=1.2;
 }
 */
