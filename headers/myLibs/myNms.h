#ifndef MYNMS_H_
#define MYNMS_H_

#include "myDataTypes.h"
#include "myLibs.h"
#include "mySort.h"

namespace my{
	
template <typename T>
_Vector< _box<T> > * nms(_Vector< _box<T> > *boxes, T overlap, int nms_limit){

	if( !boxes ) return NULL;
	if( boxes->length == 0 ) return boxes;

	int boxcache = boxes->length;
	int N = boxes->length;
	_Vector<T> *s;
	_Vector<int> I, *pick;
	T *x1, *x2, *y1, *y2, *area;
	T xx1, xx2, yy1, yy2, w, h, inter, o1, o2;
	int i;
	_Vector< _box<T> > tboxes = *boxes;

	if( N > nms_limit ){

		s = createVector<T>(boxes->length);
		for( int j=0; j<s->length; j++ ){  s->data[j] = boxes->data[j].s; }

		sortArray(s, &I);
		createVector(&tboxes, nms_limit);
		
		for( int j=boxes->length-nms_limit, i=0; j<boxes->length; j++, i++ ){  
			
			tboxes.data[i] = boxes->data[(int)I.data[j]];
			boxes->data[(int)I.data[j]].xy = NULL;
			I.data[i] = i;
		}

		freeBoxes(boxes, false);		
		I.length = nms_limit;
		N = nms_limit;
	}
	else {

		s = createVector<T>(boxes->length);
		for( int j=0; j<s->length; j++ ){  s->data[j] = boxes->data[j].s;  }
		sortArray(s, &I);
	}
	s = freeVector(s, true);

	x1 = (T*) malloc(N*sizeof(T));
	y1 = (T*) malloc(N*sizeof(T));
	x2 = (T*) malloc(N*sizeof(T));
	y2 = (T*) malloc(N*sizeof(T));
	area = (T*) malloc(N*sizeof(T));

	for( int nb=0; nb<N; nb++ ){

		x1[nb] = arraymin(tboxes.data[nb].xy->data, tboxes.data[nb].xy->length, 0, tboxes.data[nb].xy->dims[0]);
		y1[nb] = arraymin(tboxes.data[nb].xy->data, tboxes.data[nb].xy->length, 1, tboxes.data[nb].xy->dims[0]);
		x2[nb] = arraymax(tboxes.data[nb].xy->data, tboxes.data[nb].xy->length, 2, tboxes.data[nb].xy->dims[0]);
		y2[nb] = arraymax(tboxes.data[nb].xy->data, tboxes.data[nb].xy->length, 3, tboxes.data[nb].xy->dims[0]);

		area[nb] = (x2[nb]-x1[nb]+1) * (y2[nb]-y1[nb]+1);
	}
	
	pick = createVector<int>(N);
	pick->length = 0;

	while( I.length > 0 ){ 

		i = I.data[I.length-1];
		pick->data[pick->length] = i;
		pick->length++;
		I.data[I.length-1] = -1;
		
		for( int j=0; j<I.length-1; j++ ){

			xx1 = max(x1[I.data[j]],x1[i]);
			yy1 = max(y1[I.data[j]],y1[i]);
			xx2 = min(x2[I.data[j]],x2[i]);
			yy2 = min(y2[I.data[j]],y2[i]);

			w = xx2 - xx1 + 1;
			if( w < 0 ){ w = 0; }

			h = yy2 - yy1 + 1;
			if( h < 0 ){ h = 0; }
			
			inter = w*h;
			o1 = inter / area[j];
			o2 = inter / area[i];

			if( o1 > overlap || o2 > overlap ){  I.data[j] = -1;  }
		}

		for( int j=0; j<I.length; j++ ){

			if( I.data[j] == -1 ){

				for( i=j+1; i<I.length && I.data[i]==-1; i++ ){}
				if( i < I.length ) { myswap(&I.data[j], &I.data[i]); }
				else {
					I.length = j;
					break;
				}
			}
		}
	}

	createVector(boxes, boxcache);
	boxes->length = pick->length;

	for( int j=0; j<pick->length; j++ ){ 
		boxes->data[j] = tboxes.data[pick->data[j]];
		tboxes.data[pick->data[j]].xy = NULL;
	}

	free(x1);
	free(y1);
	free(x2);
	free(y2);
	free(area);
	freeVector(&I, false);
	freeBoxes(&tboxes, false);
	freeVector(pick, true);

	return boxes;
}

//---------------------------------------------------------------------------------------------
//--------------------------------------------- NMS DYNAMIC THRESHOLD -------------------------
//---------------------------------------------------------------------------------------------
template <typename T>
_Vector< _box<T> > * nmsRatio(_Vector< _box<T> > *boxes, T overlap, T threshold, float ratio){

	if( !boxes ) return NULL;
	if( boxes->length == 0 ) return boxes;

	int boxcache = boxes->length;
	_Vector<T> *s;
	_Vector<int> I, *pick;
	T *x1, *x2, *y1, *y2, *area;
	T xx1, xx2, yy1, yy2, w, h, inter, o1, o2;
	int i, smax, imax;
	_Vector< _box<T> > *tboxes;


	s = createVector<T>(boxes->length);
	for( int j=0; j<s->length; j++ ){  s->data[j] = boxes->data[j].s; }

	sortArray(s, &I);

	smax = (s->data[s->length-1] - threshold) * ratio;

	for( imax=s->length-2; imax>=0; imax-- ){ if( smax >= (s->data[imax] - threshold) ) break; }
	imax++;

	tboxes = createVector< _box<T> >(s->length - imax);

	for( int j=imax, i=0; j<s->length; j++, i++ ){

		tboxes->data[i] = boxes->data[(int)I.data[j]];
		boxes->data[(int)I.data[j]].xy = NULL;
		I.data[i] = i;
	}

	freeBoxes(boxes, false);
	I.length = tboxes->length;

	s = freeVector(s, true);

	x1 = (T*) malloc(tboxes->length*sizeof(T));
	y1 = (T*) malloc(tboxes->length*sizeof(T));
	x2 = (T*) malloc(tboxes->length*sizeof(T));
	y2 = (T*) malloc(tboxes->length*sizeof(T));
	area = (T*) malloc(tboxes->length*sizeof(T));

	for( int nb=0; nb<tboxes->length; nb++ ){

		x1[nb] = arraymin(tboxes->data[nb].xy->data, tboxes->data[nb].xy->length, 0, tboxes->data[nb].xy->dims[0]);
		y1[nb] = arraymin(tboxes->data[nb].xy->data, tboxes->data[nb].xy->length, 1, tboxes->data[nb].xy->dims[0]);
		x2[nb] = arraymax(tboxes->data[nb].xy->data, tboxes->data[nb].xy->length, 2, tboxes->data[nb].xy->dims[0]);
		y2[nb] = arraymax(tboxes->data[nb].xy->data, tboxes->data[nb].xy->length, 3, tboxes->data[nb].xy->dims[0]);

		area[nb] = (x2[nb]-x1[nb]+1) * (y2[nb]-y1[nb]+1);
	}

	pick = createVector<int>(tboxes->length);
	pick->length = 0;

	while( I.length > 0 ){

		i = I.data[I.length-1];
		pick->data[pick->length] = i;
		pick->length++;
		I.data[I.length-1] = -1;

		for( int j=0; j<I.length-1; j++ ){

			xx1 = max(x1[I.data[j]],x1[i]);
			yy1 = max(y1[I.data[j]],y1[i]);
			xx2 = min(x2[I.data[j]],x2[i]);
			yy2 = min(y2[I.data[j]],y2[i]);

			w = xx2 - xx1 + 1;
			if( w < 0 ){ w = 0; }

			h = yy2 - yy1 + 1;
			if( h < 0 ){ h = 0; }
			
			inter = w*h;
			o1 = inter / area[j];
			o2 = inter / area[i];

			if( o1 > overlap || o2 > overlap ){  I.data[j] = -1;  }
		}

		for( int j=0; j<I.length; j++ ){

			if( I.data[j] == -1 ){

				for( i=j+1; i<I.length && I.data[i]==-1; i++ ){}
				if( i < I.length ) { myswap(&I.data[j], &I.data[i]); }
				else {
					I.length = j;
					break;
				}
			}
		}
	}

	boxes = createVector<_box<T> >(boxes, boxcache);
	boxes->length = pick->length;

	for( int j=0; j<pick->length; j++ ){
		boxes->data[j] = tboxes->data[pick->data[j]];
		tboxes->data[pick->data[j]].xy = NULL;
	}

	free(x1);
	free(y1);
	free(x2);
	free(y2);
	free(area);
	freeVector(&I, false);
	freeBoxes(tboxes, true);
	freeVector(pick, true);

	return boxes;
}

} // end of namespace

#endif
