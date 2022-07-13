/*	OpticalFlow.c -- A sample Igor external operation

	OpticalFlow is a very simple Igor XOP (external operation) designed to show you how to get
	started writing XOPs.
	
	The syntax for OpticalFlow is:
		OpticalFlow wave
	
	It adds 1 to the specified wave.
	It generates an error message if the wave is complex or text (not numeric).
	
	HR, 091021
		Updated for 64-bit compatibility.

	HR, 2013-02-08
		Updated for Xcode 4 compatibility. Changed to use XOPMain instead of main.
		As a result the XOP now requires Igor Pro 6.20 or later.

	HR, 2018-05-04
		Recompiled with XOP Toolkit 8 which supports long object names.
		As a result the XOP now requires Igor Pro 8.00 or later.
*/


#include <opencv2/core.hpp> //order of imports important
#include <opencv2/video.hpp>
#include <opencv2/opencv.hpp>

#include "XOPStandardHeaders.h"// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h
#include "OpticalFlow.h"


// Global Variables (none)

using namespace cv;
using namespace std;




// Operation template: FarnebackOpticalFlow /PS=number:pyr_scale /L=number:levels /W=number:winsize /N=number:iterations /G wave:waveH

// Runtime param structure for FarnebackOpticalFlow operation.
#pragma pack(2)    // All structures passed to Igor are two-byte aligned.
struct FarnebackOpticalFlowRuntimeParams {
    // Flag parameters.

    // Parameters for /PS flag group.
    int PSFlagEncountered;
    double pyr_scale;
    int PSFlagParamsSet[1];

    // Parameters for /L flag group.
    int LFlagEncountered;
    double levels;
    int LFlagParamsSet[1];

    // Parameters for /W flag group.
    int WFlagEncountered;
    double winsize;
    int WFlagParamsSet[1];

    // Parameters for /N flag group.
    int NFlagEncountered;
    double iterations;
    int NFlagParamsSet[1];

    // Parameters for /G flag group.
    int GFlagEncountered;
    // There are no fields for this group because it has no parameters.

    // Parameters for /POLY flag group.
    int POLYFlagEncountered;
    double poly_n;
    double poly_sigma;
    int POLYFlagParamsSet[2];

    // Main parameters.

    // Parameters for simple main group #0.
    int waveHEncountered;
    waveHndl waveH;
    int waveHParamsSet[1];

    // These are postamble fields that Igor sets.
    int calledFromFunction;                    // 1 if called from a user function, 0 otherwise.
    int calledFromMacro;                    // 1 if called from a macro, 0 otherwise.
};
typedef struct FarnebackOpticalFlowRuntimeParams FarnebackOpticalFlowRuntimeParams;
typedef struct FarnebackOpticalFlowRuntimeParams* FarnebackOpticalFlowRuntimeParamsPtr;
#pragma pack()    // Reset structure alignment to default.

extern "C" int
ExecuteFarnebackOpticalFlow(FarnebackOpticalFlowRuntimeParamsPtr p)
{
    int err = 0;

    // Flag parameters.
    int flags=0;

    if (p->PSFlagEncountered) {
        // Parameter: p->pyr_scale
    }
    else{
        p->pyr_scale = 0.5;
    }

    if (p->LFlagEncountered) {
        // Parameter: p->levels
    }
    else{
        p->levels = 3;
    }

    if (p->WFlagEncountered) {
        // Parameter: p->winsize
    }
    else{
        p->winsize=15;
    }

    if (p->NFlagEncountered) {
        // Parameter: p->iterations
    }
    else{
        p->iterations=3;
    }

    if (p->GFlagEncountered) {
        flags = OPTFLOW_FARNEBACK_GAUSSIAN;
    }
    
    if (p->POLYFlagEncountered) {
            // Parameter: p->poly_n

            // Parameter: p->poly_sigma
    }
    else{
        p->poly_n = 5;
        p->poly_sigma = 1.2;
    }
    
    if (p->waveHEncountered) {
        
        waveHndl images=p->waveH;
        if(images == NULL){
            return NULL_WAVE_OP;
        }
        
        int type=WaveType(images);
        if(type != (NT_I8 | NT_UNSIGNED)){
            return NT_FNOT_AVAIL;
        }
        
        int numDimensions;
        CountInt dimensionSizes[MAX_DIMENSIONS+1];
        int result;
        
        if(result=MDGetWaveDimensions(images, &numDimensions, dimensionSizes)){
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
    
        if (result=MDAccessNumericWaveData(images,kMDWaveAccessMode0,&dataOffset)){
            return result;
        }
        double* dp = (double*)((char*)(*images) + dataOffset);
        unsigned char* ucp = (unsigned char*)dp;
        
        IndexInt frame;
        
        Mat flow((int) width,(int) height, CV_32FC2);
        
        waveHndl outWave_X;
        char outWaveName[MAX_OBJ_NAME+1];
        CountInt outDimensionSizes[MAX_DIMENSIONS+1] = {0};
       
        strcpy(outWaveName, "M_FarnebackOpticalFlow_X");
        outDimensionSizes[ROWS] = width;
        outDimensionSizes[COLUMNS] = height;
        outDimensionSizes[LAYERS] = frames -1;
        
    
        if(result=MDMakeWave(&outWave_X, outWaveName, NULL, outDimensionSizes, NT_FP32, 1)){
            return result;
        }

        waveHndl outWave_Y;
        strcpy(outWaveName, "M_FarnebackOpticalFlow_Y");
        if (result=MDMakeWave(&outWave_Y, outWaveName, NULL, outDimensionSizes, NT_FP32, 1)){
            return result;
        }
        
        double deltaX;
        double x0;
        char units[MAX_UNIT_CHARS+1];
        
        for(int i=0;i<2;i+=1){
            if(result=MDGetWaveScaling(images, i, &deltaX, &x0)){
                return result;
            }
            if(result = MDSetWaveScaling(outWave_X, i, &deltaX, &x0)){
                return result;
            }
            if(result = MDSetWaveScaling(outWave_Y, i, &deltaX, &x0)){
                return result;
            }
            if(result=MDGetWaveUnits(images, i, units)){
                return result;
            }
            if(result = MDSetWaveUnits(outWave_X, i, units)){
                return result;
            }
            if(result = MDSetWaveUnits(outWave_Y, i, units)){
                return result;
            }
            
        }

        BCInt outputDataOffset;
        
        if (result=MDAccessNumericWaveData(outWave_X,kMDWaveAccessMode0,&outputDataOffset)){
            return result;
        }
        
        float *fp_X = (float*)((char*)(*outWave_X) + outputDataOffset);
        
        if (result=MDAccessNumericWaveData(outWave_Y,kMDWaveAccessMode0,&outputDataOffset)){
            return result;
        }
        
        float *fp_Y = (float*)((char*)(*outWave_Y) + outputDataOffset);
        
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

            
            calcOpticalFlowFarneback(prev, next, flow, p->pyr_scale, p->levels, p->winsize, p->iterations, p->poly_n, p->poly_sigma, flags);

            
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
        
        dp=NULL;
        ucp=NULL;
        fp_Y=NULL;
        fp_X=NULL;
        offset=NULL;
        offset_out_Y=NULL;
        offset_out_X=NULL;
    
    }

    return err;
}

static int
RegisterFarnebackOpticalFlow(void)
{
    const char* cmdTemplate;
    const char* runtimeNumVarList;
    const char* runtimeStrVarList;

    // NOTE: If you change this template, you must change the FarnebackOpticalFlowRuntimeParams structure as well.
    cmdTemplate = "FarnebackOpticalFlow /PS=number:pyr_scale /L=number:levels /W=number:winsize /N=number:iterations /G /POLY={number:poly_n, number:poly_sigma} wave:waveH";
    runtimeNumVarList = "";
    runtimeStrVarList = "";
    return RegisterOperation(cmdTemplate, runtimeNumVarList, runtimeStrVarList, sizeof(FarnebackOpticalFlowRuntimeParams), (void*)ExecuteFarnebackOpticalFlow, 0);
}


static int
RegisterOperations(void)		// Register any operations with Igor.
{
	int result;
	
	// Register OpticalFlow operation.
	if (result = RegisterFarnebackOpticalFlow())
		return result;
	
	// There are no more operations added by this XOP.
		
	return 0;
}

/*	XOPEntry()

	This is the entry point from the host application to the XOP for all
	messages after the INIT message.
*/
extern "C" void
XOPEntry(void)
{	
	XOPIORecResult result = 0;
	
	// We don't need to handle any messages for this XOP.
	// switch (GetXOPMessage()) {
	// }

	SetXOPResult(result);
}

/*	XOPMain(ioRecHandle)

	This is the initial entry point at which the host application calls XOP.
	The message sent by the host must be INIT.
	
	XOPMain does any necessary initialization and then sets the XOPEntry field of the
	ioRecHandle to the address to be called for future messages.
*/
HOST_IMPORT int
XOPMain(IORecHandle ioRecHandle)
{
	int result;
	
	XOPInit(ioRecHandle);				// Do standard XOP initialization.
	SetXOPEntry(XOPEntry);				// Set entry point for future calls.
	
	if (igorVersion < 800) {			// XOP Toolkit 8.00 or later requires Igor Pro 8.00 or later
		SetXOPResult(OLD_IGOR);			// OLD_IGOR is defined in OpticalFlow.h and there are corresponding error strings in OpticalFlow.r and OpticalFlowWinCustom.rc.
		return EXIT_FAILURE;
	}

	if (result = RegisterOperations()) {
		SetXOPResult(result);
		return EXIT_FAILURE;
	}
	
	SetXOPResult(0);
	return EXIT_SUCCESS;
}
