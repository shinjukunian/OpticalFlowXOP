//
//  OpticalFlowCalculation.cpp
//  OpticalFlow-64
//
//  Created by Morten Bertz on 2022/07/22.
//

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include "OpticalFlowCalculation.hpp"
#include "Helper.hpp"

using namespace cv;


int calculateOpticalFlow(waveHndl inWave, Params *params, bool usePixel){
    
    int type=WaveType(inWave);
    if(type != (NT_I8 | NT_UNSIGNED)){
        return NT_FNOT_AVAIL;
    }
    
    int numDimensions;
    CountInt dimensionSizes[MAX_DIMENSIONS+1];
    int result;
    
    if(result=MDGetWaveDimensions(inWave, &numDimensions, dimensionSizes)){
        return result;
    }
    
    if(numDimensions != 3){
        return kNeeds3DWave;
    }
    
    CountInt width=dimensionSizes[ROWS];
    CountInt height=dimensionSizes[COLUMNS];
    CountInt frames=dimensionSizes[LAYERS];
    
    if(frames < 2){
        return kBadNumberOfLayers;
    }
    
    
    
    BCInt dataOffset;
    
    if(result=MDGetWaveDimensions(inWave, &numDimensions, dimensionSizes)){
        return result;
    }
    
    if (result=MDAccessNumericWaveData(inWave,kMDWaveAccessMode0,&dataOffset)){
        return result;
    }
    
    unsigned char* ucp = (unsigned char*)((unsigned char*)(*inWave) + dataOffset);
    
    Mat flow;//((int) width,(int) height, CV_32FC2);
    
    waveHndl outX;
    waveHndl outY;
    const char* xName;
    const char* yName;
    
    switch (params->method) {
        case OpticalFlowMethodFarneback:
            xName="M_FarnebackOpticalFlow_X";
            yName="M_FarnebackOpticalFlow_Y";
            break;
        case OpticalFlowMethodDIS:
            xName="M_DISOpticalFlow_X";
            yName="M_DISOpticalFlow_Y";
            break;
        default:
            break;
    }
    
    if (result=makeOutputWaves(inWave, &outX, xName, &outY, yName, usePixel)){
        return result;
    }
    
    double deltaX_X;
    double deltaX_Y;
    double x0;
    
    if(result=MDGetWaveScaling(inWave, 0, &deltaX_X, &x0)){
        return result;
    }
    if(result=MDGetWaveScaling(inWave, 1, &deltaX_Y, &x0)){
        return result;
    }
    
        
    if (result=MDAccessNumericWaveData(outX,kMDWaveAccessMode0,&dataOffset)){
        return result;
    }
    
    float *fp_X = (float*)((char*)(*outX) + dataOffset);
    
    if (result=MDAccessNumericWaveData(outY,kMDWaveAccessMode0,&dataOffset)){
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
    
    IndexInt frame;
    
    cv::Ptr<DenseOpticalFlow> algorithm;
    
    if (params->method == OpticalFlowMethodDIS) {
        algorithm = DISOpticalFlow::create(params->values[0].val.ival);
    }
    
    for(frame=0;frame<frames-1;frame+=1){
        
        cv::Mat prev = cv::Mat((int) width,(int) height,CV_8UC1,offset);
        
        offset = &ucp[width * height * (frame+1)];
        cv::Mat next = cv::Mat((int) width,(int) height,CV_8UC1,offset);

        switch (params->method) {
            case OpticalFlowMethodFarneback:
                calcOpticalFlowFarneback(prev, next, flow,  params->values[0].val.dval, params->values[1].val.ival, params->values[2].val.ival, params->values[3].val.ival, params->values[4].val.ival, params->values[5].val.dval, params->values[6].val.ival);
                break;
            case OpticalFlowMethodDIS:
                algorithm->calc(prev, next, flow);
                break;
            default:
                break;
        }
        

        split(flow, flow_parts);
        
        if (usePixel == 0) {
            flow_parts[0] *= deltaX_X;
            flow_parts[1] *= deltaX_Y;
        }
        
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
    
    if(algorithm){
        algorithm->collectGarbage();
    }
    
    return result;
    
}
