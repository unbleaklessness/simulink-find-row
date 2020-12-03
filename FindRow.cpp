#define S_FUNCTION_NAME FindRow
#define S_FUNCTION_LEVEL 2

#define IX(row, column) (row + column * port0Dimensions[0])

#include <vector>
#include <algorithm>

#include "simstruc.h"

#define MDL_SET_INPUT_PORT_DIMENSION_INFO
static void mdlSetInputPortDimensionInfo(SimStruct *S, int_T port, const DimsInfo_T *info) {
	ssSetInputPortMatrixDimensions(S, port, info->dims[0], info->dims[1]);
}

#define MDL_SET_OUTPUT_PORT_DIMENSION_INFO
static void mdlSetOutputPortDimensionInfo(SimStruct *S, int_T port, const DimsInfo_T *info) {}

static void mdlInitializeSizes(SimStruct *S) {

	if (!ssSetNumInputPorts(S, 2)) {
		return;
	}

	ssSetInputPortDirectFeedThrough(S, 0, 1);
	if(!ssSetInputPortDimensionInfo(S, 0, DYNAMIC_DIMENSION)) {
		return;
	}

	ssSetInputPortDirectFeedThrough(S, 1, 1);
	if(!ssSetInputPortDimensionInfo(S, 1, DYNAMIC_DIMENSION)) {
		return;
	}
	
	if (!ssSetNumOutputPorts(S, 1)) {
		return;
	}

	ssSetOutputPortMatrixDimensions(S, 0, 1, 1);

	ssSetNumSampleTimes(S, 1);

	ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE);
}

static void mdlInitializeSampleTimes(SimStruct *S) {
	ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0);
}

static void mdlOutputs(SimStruct *S, int_T id) {}

#define MDL_START
static void mdlStart(SimStruct *S)
{
	int_T *port0Dimensions = ssGetInputPortDimensions(S, 0);
	int_T *port1Dimensions = ssGetInputPortDimensions(S, 1);

	if (port0Dimensions[0] < 2 || 
		port1Dimensions[0] != 1 ||
		port1Dimensions[1] != port0Dimensions[1])
	{
		ssSetErrorStatus(S, "FindRow: Error: Input dimensions are incorrect.");
	}
}

#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid) {

	static int_T *port0Dimensions = ssGetInputPortDimensions(S, 0);
	static int_T *port1Dimensions = ssGetInputPortDimensions(S, 1);
	
	static std::vector<std::vector<real_T>> vector;
	
	static bool cached = false;

	if (!cached) {

		for (size_t i = 0; i < port0Dimensions[0]; i++) {
			std::vector<real_T> thisVector;
			for (size_t j = 0; j < port0Dimensions[1]; j++) {
				thisVector.push_back(*ssGetInputPortRealSignalPtrs(S, 0)[IX(i, j)]);
			}
			vector.push_back(thisVector);
		}

		cached = true;
	}

	std::vector<real_T> thisVector;
	for (size_t i = 0; i < port1Dimensions[1]; i++) {
		thisVector.push_back(*ssGetInputPortRealSignalPtrs(S, 1)[i]);
	}

	auto it = std::upper_bound(vector.begin(), vector.end(), thisVector);

	if (it == vector.end()) {
		std::advance(it, -1);
	}

	*ssGetOutputPortRealSignal(S, 0) = it - vector.begin();
}

static void mdlTerminate(SimStruct *S) {}

#ifdef MATLAB_MEX_FILE
#include "simulink.c"
#else
#include "cg_sfun.h"
#endif
