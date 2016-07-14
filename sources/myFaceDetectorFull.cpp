#define OMP_ENABLE
//#define TIME_PROFILE
//#define PRINT_ERRORS

#include <stdio.h>
#include <iostream>
#include <signal.h>

#include <myLibs/myDataTypes.h>
#include <myLibs/myLibs.h>
#include <xmlLibs/xmlRead.h>
#include <openCV/myReadImage.h>
#include <myLibs/mySettings.h>
#include <myIOS.h>
#include <myErrorHandler.h>

#include "myFaceDetector.h"

#ifdef TIME_PROFILE
	#include <myProfiler.h>
#endif
#ifdef OMP_ENABLE
	#include <omp.h>
#endif

using namespace std;
using namespace my;

void sig_handler(int signum)
{
	#ifdef PRINT_ERRORS
    printf("Error %d : Unknown OS Error (OS signal)!\n", SIGSEGV);
	#endif
	exit( -1*SIGSEGV );
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------MAIN FUNCTION --------------------------------
//--------------------------------------------------------------------------------------------
int main(int argc, char *argv[]){

	int detnum = 0;	
	signal(SIGSEGV, sig_handler);	// Segmentation fault (core dump) OS Signal handler
	signal(SIGABRT, sig_handler);	// Aborted (core dump)

try{
	#ifdef OMP_ENABLE
		omp_set_nested(true);
	#endif;
	#ifdef TIME_PROFILE
		_TimeProfiler *tprofiler = tprofiler = createTimeProfiler(2);
		int readData_timer = timeProfilerCreateTimer(tprofiler, 1);
		int faceDetect_timer = timeProfilerCreateTimer(tprofiler, 2);
	#endif
	
	_Vector< _Vector<char> > *args = getArguments(argv, argc);

	while( args->length < 2 || args->length > 3 ){

		char arguments[200];
		printf("Please set your arguments [image model settings]: ");
		fgets(arguments, 199, stdin);
		args = getArguments(arguments);
	} 

	char *image_path = args->data[0].data;
	char *model_path = args->data[1].data;
	char *settings_path = ( args->length > 2 ) ? args->data[2].data : NULL;

	#ifdef TIME_PROFILE
		timeProfilerStart(tprofiler, readData_timer);
	#endif
	
	// Read Image
	_Array<uint8> *image = readImage<uint8>(image_path);
	if( !image ) throw ERROR_READ_IMAGE_CODE;
	
	// Read Model
	_model<float> *model = readXMLModel<float>(model_path);
	if( !model ) throw ERROR_READ_MODEL_CODE;
	
	// Read Settings
	_settings<float> *settings = readXMLSettings<float>(settings_path);
	if( !settings ) settings = defaultSettings<float>();
	model = loadSettings2Model(model, settings);
	if( !model || !settings ) throw ERROR_READ_SETTINGS_CODE;
	
	#ifdef TIME_PROFILE
		timeProfilerStop(tprofiler, readData_timer);
	#endif

	#ifdef TIME_PROFILE
		timeProfilerStart(tprofiler, faceDetect_timer);
	#endif
	
	// Face Detection call
	_Vector< _box<float> > *boxes = faceDetector(image, model);
	if( !boxes ) throw ERROR_FACE_DETECTION_CODE;
	
	#ifdef TIME_PROFILE
		timeProfilerStop(tprofiler, faceDetect_timer);
	#endif

	#ifdef TIME_PROFILE
		printf("TSM Face Detection System Time Profile\n");
		printf("1. Read Data      :%.3fsec\n", (double) tprofiler->totalTime[readData_timer]);
		printf("2. Face Detection :%.3fsec\n", (double) tprofiler->totalTime[faceDetect_timer]);
	#endif
	
	// Return Results
	if( !setOutput(boxes, settings, image_path) ) throw ERROR_OUTPUT_CODE;
	
	detnum = boxes->length;
	freeModel(model, true);
	freeArray(image, true);
	freeBoxes(boxes, true);
	free(settings);
}
catch(int code){
	#ifdef PRINT_ERRORS
	printf("TSM System Error Code %d!\n", code);
	#endif
	return -1*code;
}
catch(std::exception& e){
	#ifdef PRINT_ERRORS
	printf("Error 20 : %s\n", e.what());
	#endif
	return -1*ERROR_XML_CODE;
}

	return detnum;
}



