#ifndef MYEXCEPTIONHANDLER_H_
#define MYEXCEPTIONHANDLER_H_

#define ERROR_OS_CODE				1
#define ERROR_RESIZE_CODE			2
#define ERROR_REDUCE_CODE			3
#define ERROR_FEATURES_CODE			4
#define ERROR_FCONV_CODE			5
#define ERROR_DT_CODE				6
#define ERROR_FIND_CODE				7
#define ERROR_BACKTRACK_CODE		8
#define ERROR_NMS_CODE				9
#define ERROR_MODEL_COMP_CODE		10
#define ERROR_CLIPBOXES_CODE		11
#define ERROR_READ_IMAGE_CODE		12
#define ERROR_READ_MODEL_CODE		13
#define ERROR_READ_SETTINGS_CODE	14
#define ERROR_READ_ARGS_CODE		15
#define ERROR_FPYRAMID_CODE			16
#define ERROR_DETECT_CODE			17
#define ERROR_FACE_DETECTION_CODE	18
#define ERROR_OUTPUT_CODE			19
#define ERROR_XML_CODE				20


/* #include <stdio.h>

using namespace std;

namespace my{
	
char *errorMessage(char *message, int code){
	
	char *em = (char*) malloc(150);
	sprintf(em, "Error %d : %s\n", code, message);
	return em;
}

} // end of namespace */

#endif