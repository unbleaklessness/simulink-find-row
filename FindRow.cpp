#define S_FUNCTION_NAME FindRow
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

#include <algorithm>
#include <vector>
#include <cstdint>

#define MDL_SET_INPUT_PORT_DIMENSION_INFO
static void mdlSetInputPortDimensionInfo(SimStruct *S, int_T port, const DimsInfo_T *info) {

	if (info->numDims != 2) {
		ssSetErrorStatus(S, "FindRow: Error: Inputs must have a matrix dimensions.");
	}

	ssSetInputPortMatrixDimensions(S, port, info->dims[0], info->dims[1]);
}

#define MDL_SET_OUTPUT_PORT_DIMENSION_INFO
static void mdlSetOutputPortDimensionInfo(SimStruct *S, int_T port, const DimsInfo_T *info) {}

static void mdlInitializeSizes(SimStruct *S) {

	// Inputs:
	// - Deltas [1, N].
	// - Minimums (first row), maximums (second row) [2, N].
	// - Numbers of elements [1, N].
	// - Input [1, N].
	// Ouput is an index of input.

	if (!ssSetNumInputPorts(S, 4)) {
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

	ssSetInputPortDirectFeedThrough(S, 2, 1);
	if(!ssSetInputPortDimensionInfo(S, 2, DYNAMIC_DIMENSION)) {
		return;
	}

	ssSetInputPortDirectFeedThrough(S, 3, 1);
	if(!ssSetInputPortDimensionInfo(S, 3, DYNAMIC_DIMENSION)) {
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
static void mdlStart(SimStruct *S) {

	int_T *port0Dimensions = ssGetInputPortDimensions(S, 0);
	int_T *port1Dimensions = ssGetInputPortDimensions(S, 1);
	int_T *port2Dimensions = ssGetInputPortDimensions(S, 2);
	int_T *port3Dimensions = ssGetInputPortDimensions(S, 3);

	if (port0Dimensions[0] != 1) {
		ssSetErrorStatus(S, "FindRow: Error: Dimensions of input port 1 must be [1, N].");
	}

	if (port1Dimensions[0] != 2 || port0Dimensions[1] != port1Dimensions[1]) {
		ssSetErrorStatus(S, "FindRow: Error: Dimensions of input port 1 and 2 must be [1, N] and [2, N].");
	}

	if (port2Dimensions[0] != 1 || port0Dimensions[1] != port2Dimensions[1]) {
		ssSetErrorStatus(S, "FindRow: Error: Dimensions of input port 1 and 3 must be [1, N] and [1, N].");
	}

	if (port3Dimensions[0] != 1 || port0Dimensions[1] != port3Dimensions[1]) {
		ssSetErrorStatus(S, "FindRow: Error: Dimensions of input port 1 and 4 must be [1, N] and [1, N].");
	}
}

#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid) {

	int_T *port1Dimensions = ssGetInputPortDimensions(S, 1);

	InputRealPtrsType input0 = ssGetInputPortRealSignalPtrs(S, 0);
	InputRealPtrsType input1 = ssGetInputPortRealSignalPtrs(S, 1);
	InputRealPtrsType input2 = ssGetInputPortRealSignalPtrs(S, 2);
	InputRealPtrsType input3 = ssGetInputPortRealSignalPtrs(S, 3);

	std::vector<real_T> temporary;

	for (size_t i = 0; i < port1Dimensions[1]; i++) {
		real_T minimum = std::round(*input1[0 + i * port1Dimensions[0]] / *input0[i]);
		real_T maximum = std::round(*input1[1 + i * port1Dimensions[0]] / *input0[i]);
		temporary.push_back(std::max(std::min(std::round(*input3[i] / *input0[i]), maximum), minimum) - minimum);
	}

	real_T index = 0;
	real_T lengths = 1;

	for (int64_t i = port1Dimensions[1] - 1; i >= 0; i--) {
		index += temporary[i] * lengths;
		lengths *= *input2[i];
	}

	*ssGetOutputPortRealSignal(S, 0) = index;
}

static void mdlTerminate(SimStruct *S) {}

#ifdef MATLAB_MEX_FILE
#include "simulink.c"
#else
#include "cg_sfun.h"
#endif
