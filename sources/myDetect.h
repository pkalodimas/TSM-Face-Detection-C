#ifndef MYDETECT_H_
#define MYDETECT_H_

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <myLibs/myDataTypes.h>
#include <myLibs/myLibs.h>
#include <myLibs/myModelComponents.h>
#include <myLibs/myResize.h>
#include <myLibs/myReduce.h>
#include <myLibs/myFeatures.h>
#include <myLibs/myFind.h>
#include <myLibs/myNms.h>
#include <myLibs/myBacktrack.h>
#include <myLibs/myfconv.h>
#include <myLibs/myshiftdt.h>

using namespace my;

template <typename T>
_Vector< _box<T> >* detect(_model<T> *model, _Array<uint8> *image){

	if( !model || !image ) return NULL;

	// Features Pyramid Stage
	_featurePyramid<T> *pyra = newFeaturePyramid<T>(true);
	
	int sbin = model->sbin;
	int interval = model->interval;
	int minlevel = model->MinLevel;
	float sc = (float) pow((float)2,(float)1/interval);
	int pads[3] = { max( model->maxsize.data[1]-2, 0)+1 , mymax( model->maxsize.data[0]-2, 0)+1, 0 };
	int max_scale = (int) ceilf(log2f( (float) image->dims[1]/model->Face_Height ) * model->interval);

	pyra->interval = interval;
	pyra->imx = image->dims[0];
	pyra->imy = image->dims[1];
	pyra->padx = pads[0]-1;
	pyra->pady = pads[1]-1;
	
	model->MinLevel = mymin(model->MinLevel, max_scale);
	if( model->MaxLevel > 0 ){
		model->MaxLevel = mymax(model->MaxLevel, model->MinLevel);
		max_scale = mymin(model->MaxLevel, max_scale);
		model->MaxLevel = max_scale;
	}else model->MaxLevel = max_scale;
	
	createVector(&pyra->feat,(int) max_scale, true);
	createVector(&pyra->scale,(int) max_scale, true);
	
	_Array<T> *im = createArray<T>(image->dims);
	for( int i=0; i<image->length; i++ ){ im->data[i] = (T) image->data[i]; }	// uint8 to double
	
	// Model Components Process
	model = modelComponents(model, pyra);
	if( !model ) throw ERROR_MODEL_COMP_CODE;
	
	// Detection Stage
	_Vector< _box<T> > *boxes = createVector< _box<T> >(model->BoxCacheSize, false);
	_Vector< _Vector< _part<T> > > *components = &model->components;
	_Vector< _filter<T> > *filters = &model->filters;
	int end = pyra->feat.length - 1;
	
	_Vector< _Vector< _Array<T>* >* > *resps = createVector(resps, pyra->feat.length, true);
	boxes->length = 0;
	
	// First Level of Pyramid
	pyra->feat.data[0] = im;
	pyra->scale.data[0] = (float) pow(2,(float)(0+interval)/interval) * sbin/2;
	_Array<T> *feats = features(pyra->feat.data[0], sbin, pads);
	resps->data[0] = featureResponce(feats, filters);			
	freeArray(feats, true);
	
	#ifdef OMP_ENABLE
	#pragma omp parallel sections shared(resps)
	{
	#pragma omp section
	{
	omp_set_num_threads(OMP_SECTION_1_CORES);
	#endif
	
	if( interval <= end ) pyra->feat.data[interval] = reduce(pyra->feat.data[0]);
						
	for( int r=1; r<=end; r++ ){
		
		if( !pyra->feat.data[r] ) pyra->feat.data[r] = resize(im, 1/pow(sc,r));
		if( !pyra->feat.data[r] ) throw ERROR_RESIZE_CODE;
		
		if( r+interval <= end ) pyra->feat.data[r+interval] = reduce(pyra->feat.data[r]);
		if( r+interval <= end && !pyra->feat.data[r+interval] ) throw ERROR_REDUCE_CODE;
		
		pyra->scale.data[i] = (float) pow(2,(float)(r+interval)/interval) * sbin/2;

		feats = features(pyra->feat.data[r], sbin, pads);
		pyra->feat.data[r] = freeArray(pyra->feat.data[r], true);
		if( !feats ) throw ERROR_FEATURES_CODE;

		resps->data[r] = featureResponce(feats, filters);					
		freeArray(feats, true);
		if( !resps->data[r] ) throw ERROR_FCONV_CODE;
	}
	
	freeArray(im, true);
	
	#ifdef OMP_ENABLE
	}// section
	#pragma omp section
	{
	#endif
	
	for( int level=0; level<=end; level++ ){

	#ifdef OMP_ENABLE
	#pragma omp flush (resps)
	#endif
		
		_Vector< _Array<T>* > *resp = resps->data[level];
		
		#ifdef OMP_ENABLE
		while( !resp ){
			#pragma omp flush (resps)
			resp = resps->data[level];
		}
		#endif

		#ifdef OMP_ENABLE
		#pragma omp parallel for num_threads(OMP_SECTION_2_CORES)
		#endif
		for( int c=0; c<components->length; c++ ){

			_Vector< _part<T> > *parts = &(components->data[c]);

			for( int k=parts->length-1; k>0; k-- ){

				_part<T> *child = &parts->data[k];
				_part<T> *parent = &parts->data[child->parent-1];
				child->level = level+1;

				if( !child->score ){  child->score = copyArray(resp->data[child->filterid-1]);  }

				child->Ix = newArray<int>(false);
				child->Iy = newArray<int>(false);

				child->score = distanceTransformation(child->score, child->score, child->w.data, child->startx, child->starty, child->step, child->Ix, child->Iy );
				if( !child->score ) throw ERROR_DT_CODE;

				if( !parent->score ) parent->score = matrixAddition(resp->data[parent->filterid-1], child->score, child->score);
				else {
					matrixAddition(parent->score, child->score, parent->score);
					freeArray(child->score, true);
				}
				child->score = NULL;
			}

			_Array<T> *rscore = matrixIncrease(parts->data[0].score, parts->data[0].w.data[0], parts->data[0].score);
			parts->data[0].score = NULL;
			parts->data[0].level = level + 1;

			_Vector<int> X, Y;
			int e = nms_find(rscore, model->thresh, model->Find_Buffer, &X, &Y);
			if( e < 0 ) throw ERROR_FIND_CODE;

			if( X.length > 0 ) {

				_Vector< _Array<T>* > *XY = backtrack(&X, &Y, parts, pyra);
				if( !XY ) throw ERROR_BACKTRACK_CODE;
				
				#ifdef OMP_ENABLE
				#pragma omp critical
				{
				#endif
				for( int i=0; i<X.length; i++ ){

					if( boxes->length == model->BoxCacheSize ) {
						boxes = nms(boxes, (T) model->Nms_Overlap, model->Nms_Limit);
						if( !boxes ) throw ERROR_NMS_CODE;
					}

					int cnt = boxes->length;
					boxes->data[cnt].c = c + 1;
					boxes->data[cnt].s = rscore->data[Y.data[i]*rscore->dims[0] + X.data[i]];
					boxes->data[cnt].level = level + 1;
					boxes->data[cnt].xy = XY->data[i];

					boxes->length++;
				}
				#ifdef OMP_ENABLE
				} //OMP Critical
				#endif
				
				freeVector(XY, true);
			}

			freeVector(&X, false);
			freeVector(&Y, false);
			freeArray(rscore, true);
			for( int i=0; i<parts->length; i++ ){
				parts->data[i].Ix = freeArray(parts->data[i].Ix, true);
				parts->data[i].Iy = freeArray(parts->data[i].Iy, true);
			}
		}

		freeArrayPointersVector(resp, true);
	}

#ifdef OMP_ENABLE	
}//OMP section
}//OMP sections
#endif

	return boxes;
}

#endif


