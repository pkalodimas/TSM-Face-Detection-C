#ifndef MYDETECT_H_
#define MYDETECT_H_

#include <stdlib.h>
#include <stdio.h>

#include <myLibs/myDataTypes.h>
#include <myLibs/myLibs.h>
#include <myLibs/myFind.h>
#include <myLibs/myNms.h>
#include <myLibs/myBacktrack.h>
#include <myLibs/myfconv.h>
#include <myLibs/myshiftdt.h>

using namespace my;

template <typename T>
_Vector< _box<T> >* detect(_model<T> *model, _featurePyramid<T> *pyra){

	if( !model || !pyra ) return NULL;

	_Vector< _part<T> > *parts;
	_Array<T> *rscore;
	_Vector<  _Array<T>* > *XY;
	_Vector<int> X, Y;
	_Vector< _box<T> > *boxes = createVector< _box<T> >(model->BoxCacheSize, false);
	_Vector< _Vector< _part<T> > > *components = &model->components;
	_Vector< _filter<T> > *filters = &model->filters;
	_Vector< _Vector< _Array<T>* >* > *resp2 = NULL;
	_Vector< _Array<T>* > *resp = NULL;
 
	resp2 = createVector(resp2, pyra->feat.length, true);
	boxes->length = 0;

	for( int c=0; c<components->length; c++ ){

		for( int level=0; level<pyra->feat.length; level++ ){

			if ( !resp2->data[level] ) {

				resp2->data[level] = featureResponce(pyra->feat.data[level], filters);
				if( !resp2->data[level] ) throw ERROR_FCONV_CODE;
				pyra->feat.data[level] = freeArray(pyra->feat.data[level], true);
			}
			
			resp = resp2->data[level];
			parts = &(components->data[c]);

			for( int k=parts->length-1; k>0; k-- ){

				_part<T> *child = &parts->data[k];
				_part<T> *parent = &parts->data[child->parent-1];
				child->level = level+1;

				if( !child->score ){  child->score = copyArray(resp->data[child->filterid-1]);  }
				
				child->Ix = newArray<int>(false);
				child->Iy = newArray<int>(false);

				child->score = dTransformation(child->score, child->score, child->w.data, child->startx, child->starty, child->step, child->Ix, child->Iy );
				if( !child->score ) throw ERROR_DT_CODE;
				
				if( !parent->score ) parent->score = matrixAddition(resp->data[parent->filterid-1], child->score, child->score);
				else {
					matrixAddition(parent->score, child->score, parent->score);
					freeArray(child->score, true);
				}
				child->score = NULL;
			}

			rscore = matrixIncrease(parts->data[0].score, parts->data[0].w.data[0], parts->data[0].score);
			parts->data[0].score = NULL;
			parts->data[0].level = level + 1;

			int e = find(rscore, model->thresh, model->Find_Buffer, &X, &Y);
			if( e < 0 ) throw ERROR_FIND_CODE;

			if( X.length > 0) {
				
				XY = backtrack(&X, &Y, parts, pyra);
				if( !XY ) throw ERROR_BACKTRACK_CODE;

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
	}

	for( int j=0; j<resp2->length; j++ ){
		resp2->data[j] = freeArrayPointersVector(resp2->data[j], true);
	}
	freeVector(resp2, true);

	return boxes;
}

#endif
