#ifndef MYFEATPYRAMID_H_
#define MYFEATPYRAMID_H_

#include <stdio.h>
#include <string.h>
#include <cmath>

#include <myLibs/myDataTypes.h>
#include <myLibs/myLibs.h>
#include <myLibs/myResize.h>
#include <myLibs/myReduce.h>
#include <myLibs/myModelComponents.h>

#include "myDetect.h"

using namespace my;

template <typename T, typename U>
_Vector< _box<T> > *featurePyramid(_Array<U> *image, _model<T> *model){

	if( !image || !model ) return NULL;

	_featurePyramid<T> *pyra = newFeaturePyramid<T>(true);
	_Vector< _box<T> > *boxes = createVector< _box<T> >(model->BoxCacheSize, false);
	boxes->length = 0;
	
	int sbin = model->sbin;
	int interval = model->interval;
	int minlevel = model->MinLevel;
	float sc = (float) pow((float)2,(float)1/interval);
	
	pyra->interval = interval;
	pyra->imx = image->dims[0];
	pyra->imy = image->dims[1];
	pyra->padx = max( model->maxsize.data[1]-2, 0) - 1;
	pyra->pady = mymax( model->maxsize.data[0]-2, 0) - 1;
	
	model = modelComponents(model, pyra);
	
	_Array<T> *im = createArray<T>(image->dims);
	for( int i=0; i<image->length; i++ ){ im->data[i] = (T) image->data[i]; }	// uint8 to double

	int max_scale = (int) ceilf(log2f( (float) image->dims[1]/model->Face_Height ) * model->interval);	
	
	model->MinLevel = mymin(model->MinLevel, max_scale);
	if( model->MaxLevel > 0 ){
		model->MaxLevel = mymax(model->MaxLevel, model->MinLevel);
		max_scale = mymin(model->MaxLevel, max_scale);
		model->MaxLevel = max_scale;
	}else model->MaxLevel = max_scale;
	
	createVector(&pyra->feat,(int) max_scale, true);
	createVector(&pyra->scale,(int) max_scale);

	for( int i=0; i<mymin(interval, max_scale); i++ ){

		pyra->feat.data[i] = resize(im, 1/pow(sc,i));
		pyra->scale.data[i] = (float) pow(2,(float)(i+interval)/interval) * sbin/2;
		if( !pyra->feat.data[i] ) throw ERROR_RESIZE_CODE;
		
		// Create next interval image if needed
		if( i+interval < max_scale ){
			
			pyra->feat.data[i+interval] = reduce(pyra->feat.data[i]);
			pyra->scale.data[i+interval] = (float) (2 * pyra->scale.data[i]);
			if( !pyra->feat.data[i+interval] ) throw ERROR_REDUCE_CODE;
		}
		
		// Call Detect stage if needed
		if( i >= minlevel ) detect(model, pyra, boxes, i);
		else pyra->feat.data[i] = freeArray(pyra->feat.data[i], true);
	}

	for( int j=mymin(interval, max_scale); j<max_scale; j++ ){

		// Create next interval image if needed
		if( j+interval < max_scale ){
			pyra->feat.data[j+interval] = reduce(pyra->feat.data[j]);
			pyra->scale.data[j+interval] = (float) (2 * pyra->scale.data[j]);
			if( !pyra->feat.data[j+interval] ) throw ERROR_REDUCE_CODE;
		}
		
		// Call Detect stage if needed
		if( j >= minlevel ) detect(model, pyra, boxes, j);
		else pyra->feat.data[j] = freeArray(pyra->feat.data[j], true);			
	}

	freeArray(im, true);

	return boxes;
}

#endif
