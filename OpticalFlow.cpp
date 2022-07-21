/*	OpticalFlow.c -- Calculates Optical Flow Fields using openCV

*/
#include <opencv2/core.hpp> //order of imports important
#include <opencv2/video.hpp>
#include "OpticalFlowCalculation.hpp"// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h
#include "OpticalFlow.h"


// Global Variables (none)

using namespace cv;
using namespace std;

// Operation template: OpticalFlowFarneback /PS=number:pyr_scale /L=number:levels /W=number:winsize /N=number:iterations /G /POLY={number:poly_n, number:poly_sigma} /PXL wave:waveH

// Runtime param structure for OpticalFlowFarneback operation.
#pragma pack(2)    // All structures passed to Igor are two-byte aligned.
struct OpticalFlowFarnebackRuntimeParams {
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

    // Parameters for /PXL flag group.
    int PXLFlagEncountered;
    // There are no fields for this group because it has no parameters.

    // Main parameters.

    // Parameters for simple main group #0.
    int waveHEncountered;
    waveHndl waveH;
    int waveHParamsSet[1];

    // These are postamble fields that Igor sets.
    int calledFromFunction;                    // 1 if called from a user function, 0 otherwise.
    int calledFromMacro;                    // 1 if called from a macro, 0 otherwise.
};
typedef struct OpticalFlowFarnebackRuntimeParams OpticalFlowFarnebackRuntimeParams;
typedef struct OpticalFlowFarnebackRuntimeParams* OpticalFlowFarnebackRuntimeParamsPtr;
#pragma pack()    // Reset structure alignment to default.

extern "C" int
ExecuteOpticalFlowFarneback(OpticalFlowFarnebackRuntimeParamsPtr p)
{
    int err = 0;

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
    
    params.method=OpticalFlowMethodFarneback;

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
    
    if (p->PXLFlagEncountered) {
        
    }
    
    if (p->waveHEncountered) {
        
        waveHndl images=p->waveH;
        if(images == NULL){
            return NULL_WAVE_OP;
        }
        
        return calculateOpticalFlow(images, &params, p->PXLFlagEncountered);
            
    }

    return err;
}

static int
RegisterOpticalFlowFarneback(void)
{
    const char* cmdTemplate;
    const char* runtimeNumVarList;
    const char* runtimeStrVarList;

    // NOTE: If you change this template, you must change the OpticalFlowFarnebackRuntimeParams structure as well.
    cmdTemplate = "OpticalFlowFarneback /PS=number:pyr_scale /L=number:levels /W=number:winsize /N=number:iterations /G /POLY={number:poly_n, number:poly_sigma} /PXL wave:waveH";
    runtimeNumVarList = "";
    runtimeStrVarList = "";
    return RegisterOperation(cmdTemplate, runtimeNumVarList, runtimeStrVarList, sizeof(OpticalFlowFarnebackRuntimeParams), (void*)ExecuteOpticalFlowFarneback, 0);
}



// Operation template: OpticalFlowDIS /PRST=number:preset /PXL wave:waveH

// Runtime param structure for OpticalFlowDIS operation.
#pragma pack(2)    // All structures passed to Igor are two-byte aligned.
struct OpticalFlowDISRuntimeParams {
    // Flag parameters.

    // Parameters for /PRST flag group.
    int PRSTFlagEncountered;
    double preset;
    int PRSTFlagParamsSet[1];

    // Parameters for /PXL flag group.
    int PXLFlagEncountered;
    // There are no fields for this group because it has no parameters.

    // Main parameters.

    // Parameters for simple main group #0.
    int waveHEncountered;
    waveHndl waveH;
    int waveHParamsSet[1];

    // These are postamble fields that Igor sets.
    int calledFromFunction;                    // 1 if called from a user function, 0 otherwise.
    int calledFromMacro;                    // 1 if called from a macro, 0 otherwise.
};
typedef struct OpticalFlowDISRuntimeParams OpticalFlowDISRuntimeParams;
typedef struct OpticalFlowDISRuntimeParams* OpticalFlowDISRuntimeParamsPtr;
#pragma pack()    // Reset structure alignment to default.

extern "C" int
ExecuteOpticalFlowDIS(OpticalFlowDISRuntimeParamsPtr p)
{
    int err = 0;
    
    Params params;
    params.count = 1;
    
    ParamItem items[1];
    params.values = items;
    params.values[0].type = ParamItem::is_int;
    params.method=OpticalFlowMethodDIS;
    
    // Flag parameters.

    if (p->PRSTFlagEncountered) {
        if(p->preset < 3 && p->preset >= 0){
            params.values[0].val.ival = (int) p->preset;
        }
        else{
            return kParameterOutOfRange;
        }
        
    }
    else{
        params.values[0].val.ival = 1;
    }

    if (p->PXLFlagEncountered) {
        
    }

    
    if(p->waveHEncountered){
        waveHndl images=p->waveH;
        if(images == NULL){
            return NULL_WAVE_OP;
        }
        
        return calculateOpticalFlow(images, &params, p->PXLFlagEncountered);
    }


    return err;
}

static int
RegisterOpticalFlowDIS(void)
{
    const char* cmdTemplate;
    const char* runtimeNumVarList;
    const char* runtimeStrVarList;

    // NOTE: If you change this template, you must change the OpticalFlowDISRuntimeParams structure as well.
    cmdTemplate = "OpticalFlowDIS /PRST=number:preset /PXL wave:waveH";
    runtimeNumVarList = "";
    runtimeStrVarList = "";
    return RegisterOperation(cmdTemplate, runtimeNumVarList, runtimeStrVarList, sizeof(OpticalFlowDISRuntimeParams), (void*)ExecuteOpticalFlowDIS, 0);
}



static int
RegisterOperations(void)		// Register any operations with Igor.
{
	int result;
	
	// Register OpticalFlow operation.
    if (result = RegisterOpticalFlowFarneback()){
		return result;
    }
    if (result = RegisterOpticalFlowDIS()){
        return result;
    }
	
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
