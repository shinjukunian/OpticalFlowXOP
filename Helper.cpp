//
//  Helper.cpp
//  OpticalFlow-64
//
//  Created by Morten Bertz on 2022/07/14.
//

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include "Helper.hpp"

using namespace cv;

int makeOutputWaves(waveHndl inWave, waveHndl *outX, const char *xName, waveHndl *outY, const char *yName){
    
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
    
    return result;
}



int calculateOpticalFlow_Farneback(waveHndl *inWave, waveHndl *outX, waveHndl *outY, Params *params){
    
    BCInt dataOffset;
    
    IndexInt frame;
    int result=0;
    
    int numDimensions;
    CountInt dimensionSizes[MAX_DIMENSIONS+1];
    
    if(result=MDGetWaveDimensions(*inWave, &numDimensions, dimensionSizes)){
        return result;
    }
    
    
    CountInt width=dimensionSizes[ROWS];
    CountInt height=dimensionSizes[COLUMNS];
    CountInt frames=dimensionSizes[LAYERS];
    
    if (result=MDAccessNumericWaveData(*inWave,kMDWaveAccessMode0,&dataOffset)){
        return result;
    }
    
    unsigned char* ucp = (unsigned char*)((unsigned char*)(*inWave) + dataOffset);
    
    Mat flow((int) width,(int) height, CV_32FC2);
    
        
    if (result=MDAccessNumericWaveData(*outX,kMDWaveAccessMode0,&dataOffset)){
        return result;
    }
    
    float *fp_X = (float*)((char*)(*outX) + dataOffset);
    
    if (result=MDAccessNumericWaveData(*outY,kMDWaveAccessMode0,&dataOffset)){
        return result;
    }
    
    float *fp_Y = (float*)((char*)(*outY) + dataOffset);
    
    float *offset_out_X;
    float *offset_out_Y;
    
    //for debugging
//        unsigned char* ucp_out = (unsigned char*)dp_Out;
//        unsigned char *offset_out_magnitude = &ucp_out[0];
//        unsigned char *offset_out_angle = &ucp_out[outDimensionSizes[ROWS] * outDimensionSizes[COLUMNS] * outDimensionSizes[LAYERS]];
    
    Mat flow_parts[2];
//        Mat magnitude, angle;
    
    unsigned char *offset = &ucp[0];
    
    
    for(frame=0;frame<frames-1;frame+=1){
        
        cv::Mat prev = cv::Mat((int) width,(int) height,CV_8UC1,offset);
        
        offset = &ucp[width * height * (frame+1)];
        cv::Mat next = cv::Mat((int) width,(int) height,CV_8UC1,offset);

        
        
        calcOpticalFlowFarneback(prev, next, flow,  params->values[0].val.dval, params->values[1].val.ival, params->values[2].val.ival, params->values[3].val.ival, params->values[4].val.ival, params->values[5].val.dval, params->values[6].val.ival);

        
        split(flow, flow_parts);
        
        //the conversion to poalr is strange, it works better when doing this in igor for visualization
//            cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, false);
        
        offset_out_X = &fp_X[width * height * frame];
        offset_out_Y = &fp_Y[width * height * frame];

//            offset_out_angle = &fp[outDimensionSizes[ROWS] * outDimensionSizes[COLUMNS] * outDimensionSizes[LAYERS] + width * height * frame];
        
//            memcpy(offset_out_magnitude, magnitude.data, width * height * sizeof(float));
//            memcpy(offset_out_angle, angle.data, width * height * sizeof(float));
        
        memcpy(offset_out_X, flow_parts[0].data, width * height * sizeof(float));
        memcpy(offset_out_Y, flow_parts[1].data, width * height * sizeof(float));
        
    }
    
    
    ucp=NULL;
    fp_Y=NULL;
    fp_X=NULL;
    offset=NULL;
    offset_out_Y=NULL;
    offset_out_X=NULL;
    
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
