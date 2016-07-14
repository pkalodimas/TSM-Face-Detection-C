#ifndef MYFEATPYRAMID_H_
#define MYFEATPYRAMID_H_

#include <stdio.h>
#include <string.h>
#include <cmath>

#include <myLibs/myDataTypes.h>
#include <myLibs/myLibs.h>
#include <myLibs/myResize.h>
#include <myLibs/myFeatures.h>
#include <myLibs/myReduce.h>

using namespace my;

template <typename T, typename U>
_featurePyramid<T> * featurePyramid(_Array<U> *image, _model<T> *model){

	if( !image || !model ) return NULL;

	_featurePyramid<T> *pyra = newFeaturePyramid<T>(true);
	
	int sbin = model->sbin;
	int interval = model->interval;
	float sc = (float) pow((float)2,(float)1/interval);
	int pads[3] = { max( model->maxsize.data[1]-2, 0)+1 , mymax( model->maxsize.data[0]-2, 0)+1, 0 };
	
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

		_Array<T> *scaled = resize(im, 1/pow(sc,i));
		if( !scaled ) throw ERROR_RESIZE_CODE;

		pyra->feat.data[i] = features(scaled, sbin, pads);
		pyra->scale.data[i] = (float) pow(2,(float)(i+interval)/interval) * sbin/2;
		if( !pyra->feat.data[i] ) throw ERROR_FEATURES_CODE;

		for( int j=i+interval; j<max_scale; j+=interval ){

			_Array<T> *reduced = reduce(scaled);
			if( !reduced ) throw ERROR_REDUCE_CODE;
			
			pyra->feat.data[j] = features(reduced, sbin, pads);
			pyra->scale.data[j] = (float) (2 * pyra->scale.data[j-interval]);
			if( !pyra->feat.data[j] ) throw ERROR_FEATURES_CODE;

			freeArray(scaled, true);
			scaled = reduced;
		}

		freeArray(scaled, true);
	}
	
	pyra->interval = interval;
	pyra->imx = image->dims[0];
	pyra->imy = image->dims[1];
	pyra->padx = pads[0]-1;
	pyra->pady = pads[1]-1;

	freeArray(im, true);

	return pyra;
}

#endif
