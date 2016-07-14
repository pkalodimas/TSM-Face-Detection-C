#ifndef MYFACEDETECTOR_H_
#define MYFACEDETECTOR_H_

#include <stdio.h>
#include <time.h>
#include <iostream>

#include <myLibs/myDataTypes.h>
#include <myLibs/myLibs.h>
#include <myLibs/myClipboxes.h>
#include <myLibs/myNms.h>

#include "myDetect.h"

using namespace my;

template <typename T>
_Vector< _box<T> > *faceDetector(_Array<uint8> *image, _model<T> *model){

	if( !image || !model ) return NULL;

	_Vector< _box<T> > *boxes = detect(model, image);
	if( !boxes ) throw ERROR_DETECT_CODE;

	boxes = clipboxes(boxes, image);
	if( !boxes ) throw ERROR_CLIPBOXES_CODE;	
	
	boxes = nms(boxes, (T) model->Nms_Overlap, model->Nms_Limit);				
	if( !boxes ) throw ERROR_NMS_CODE;	

	return boxes;
}

#endif
