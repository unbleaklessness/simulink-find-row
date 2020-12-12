#define S_FUNCTION_NAME FindRow
#define S_FUNCTION_LEVEL 2

#include "simstruc.h"

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
static void mdlStart(SimStruct *S) {

	int_T *port0Dimensions = ssGetInputPortDimensions(S, 0);
	int_T *port1Dimensions = ssGetInputPortDimensions(S, 1);

	if (port1Dimensions[0] != 1) {
		ssSetErrorStatus(S, "FindRow: Error: Dimensions of input port 2 must be [1, N].");
	}

	if (port0Dimensions[1] != port1Dimensions[1]) {
		ssSetErrorStatus(S, "FindRow: Error: Dimensions of input ports 1 and 2 must be [M, N] and [1, N].");
	}
}

#define MDL_UPDATE
static void mdlUpdate(SimStruct *S, int_T tid) {

	int_T *port0Dimensions = ssGetInputPortDimensions(S, 0);
	int_T port0Rows = port0Dimensions[0];
	int_T port0Columns = port0Dimensions[1];

	InputRealPtrsType input0 = ssGetInputPortRealSignalPtrs(S, 0);
	InputRealPtrsType input1 = ssGetInputPortRealSignalPtrs(S, 1);

	size_t count = port0Rows - 1;
	size_t first = 0;
	size_t it;
	size_t step;
	bool flag;

	while (count > 0) {
		it = first;
		step = count / 2;
		it += step;
		flag = false;
		for (size_t i = 0; i < port0Columns; i++) {
			if (*input1[i] >= *input0[it + i * port0Rows]) {
				flag = true;
				break;
			}
		}
		if (flag) {
			first = ++it;
			count -= step + 1;
		} else {
			count = step;
		}
	}

	*ssGetOutputPortRealSignal(S, 0) = first;
}

static void mdlTerminate(SimStruct *S) {}

#ifdef MATLAB_MEX_FILE
#include "simulink.c"
#else
#include "cg_sfun.h"
#endif
