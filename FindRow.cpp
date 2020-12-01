#define S_FUNCTION_NAME FindRow
#define S_FUNCTION_LEVEL 2

#define IX(row, column) (row + column * port0Dimensions[0])

#include <vector>
#include <algorithm>

#include "simstruc.h"

std::vector<std::vector<real_T>> vector;

bool matrixCached = false;

int_T *port0Dimensions = nullptr;
int_T *port1Dimensions = nullptr;

#define MDL_SET_INPUT_PORT_DIMENSION_INFO
void mdlSetInputPortDimensionInfo(SimStruct *S, int_T port, const DimsInfo_T *info) {
	ssSetInputPortMatrixDimensions(S, port, info->dims[0], info->dims[1]);
}

#define MDL_SET_OUTPUT_PORT_DIMENSION_INFO
void mdlSetOutputPortDimensionInfo(SimStruct *S, int_T port, const DimsInfo_T *info) {
	ssSetInputPortMatrixDimensions(S, port, 1, 1);
}

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
	port0Dimensions = ssGetInputPortDimensions(S, 0);
	port1Dimensions = ssGetInputPortDimensions(S, 1);

	if (port0Dimensions[0] < 2 || 
		port0Dimensions[1] < 2 ||
		port1Dimensions[0] != 1 ||
		port1Dimensions[1] != port0Dimensions[1])
	{
		ssSetErrorStatus(S, "FindRow: Error: Input dimensions are incorrect.");
	}
}

#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid) {

	if (!matrixCached) {

		for (size_t i = 0; i < port0Dimensions[0]; i++) {
			std::vector<real_T> thisVector;
			for (size_t j = 0; j < port0Dimensions[1]; j++) {
				real_T value = *ssGetInputPortRealSignalPtrs(S, 0)[IX(i, j)];
				thisVector.push_back(value);
			}
			vector.push_back(thisVector);
		}

		matrixCached = true;
	}

	std::vector<real_T> thisVector;
	for (size_t i = 0; i < port1Dimensions[1]; i++) {
		real_T value = *ssGetInputPortRealSignalPtrs(S, 1)[i];
		thisVector.push_back(value);
	}

	auto it = std::upper_bound(vector.begin(), vector.end(), thisVector);
	if (it == vector.end()) {
		*ssGetOutputPortRealSignal(S, 0) = -1;
	} else {
		*ssGetOutputPortRealSignal(S, 0) = it - vector.begin() + 1;
	}
}

static void mdlTerminate(SimStruct *S) {
	vector.clear();
	matrixCached = false;
	port0Dimensions = nullptr;
	port1Dimensions = nullptr;
}

#ifdef MATLAB_MEX_FILE
#include "simulink.c"
#else
#include "cg_sfun.h"
#endif
