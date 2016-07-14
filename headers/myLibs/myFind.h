#ifndef MYFIND_H_
#define MYFIND_H_

#include <stdio.h>
#include <string.h>

#include "myDataTypes.h"
#include "myLibs.h"

namespace my{

//--------------------------------------------- FIND V1.0 ------------------------------------

template <typename T, typename I>
int find(_Array<T> *table, T threshold, int fbuffer, _Vector<I> *x, _Vector<I> *y){
	
	if( !table || !x || !y ) return -1;

	T *ptr;
	int cnt = 0;

	x = createVector(x, 0);
	y = createVector(y, 0);

	ptr = table->data;
	for( int i=0; i<table->dims[1]; i++ ){

		for( int j=0; j<table->dims[0]; j++, ptr++ ){

			if( *ptr >= threshold ) {

				if( cnt == x->length ){
					cnt += fbuffer;
					x->data = (int*) realloc(x->data, cnt*sizeof(int));
					y->data = (int*) realloc(y->data, cnt*sizeof(int));
				}
				x->data[x->length] = j;
				y->data[x->length] = i;
				x->length++;
			}
		}
	}
	y->length = x->length;

	return x->length;
}

//--------------------------------------------- FIND V2.0 ------------------------------------

template <typename T>
_Array<T> *clearArea(_Array<T> *area, int x, int y, T thresh, T rval){

	if( !area ) return area;
	area->data[y*area->dims[0]+x] = rval;

	if( x < 1 || y < 1 || x > (area->dims[0]-2) || y > (area->dims[1]-2) ) return area;

	if( area->data[((y-1)*area->dims[0])+x-1] >= thresh ) clearArea(area, x-1, y-1, thresh, rval);
	if( area->data[((y-1)*area->dims[0])+x] >= thresh ) clearArea(area, x, y-1, thresh, rval);
	if( area->data[((y-1)*area->dims[0])+x+1] >= thresh ) clearArea(area, x+1, y-1, thresh, rval);

	if( area->data[(y*area->dims[0])+x-1] >= thresh ) clearArea(area, x-1, y, thresh, rval);
	if( area->data[(y*area->dims[0])+x+1] >= thresh ) clearArea(area, x+1, y, thresh, rval);

	if( area->data[((y+1)*area->dims[0])+x-1] >= thresh ) clearArea(area, x-1, y+1, thresh, rval);
	if( area->data[((y+1)*area->dims[0])+x] >= thresh ) clearArea(area, x, y+1, thresh, rval);
	if( area->data[((y+1)*area->dims[0])+x+1] >= thresh ) clearArea(area, x+1, y+1, thresh, rval);

	return area;
}

template <typename T, typename I>
int nms_find(_Array<T> *table, T threshold, int fbuffer, _Vector<I> *X, _Vector<I> *Y){

	if( !table || !X || !Y ) return -1;

	T *ptr;
	int cnt = 0;
	int len = 0;
	T rval = (T) (threshold > 0)? 0 : 2*threshold;

	_Vector<T> *s = NULL;

	X = createVector(X, 0);
	Y = createVector(Y, 0);

	do {
		ptr = table->data;
		len = 0;

		for( int i=0; i<table->dims[1]; i++ ){

			for( int j=0; j<table->dims[0]; j++, ptr++ ){

				if( *ptr >= threshold ) {

					if( cnt == X->length ){
						cnt += fbuffer;
						X->data = (I*) realloc(X->data, cnt*sizeof(I));
						Y->data = (I*) realloc(Y->data, cnt*sizeof(I));
					}
					X->data[X->length] = j;
					Y->data[X->length] = i;
					X->length++;
					len++;
				}
			}
		}
		Y->length = X->length;
		
		if( len == 0) break;
		if( !s ) {
			s = createVector<T>(len);
			s->length = 0;
		}

		int start = X->length - len;
		int maxi = start;
		int x = X->data[start];
		int y = Y->data[start];
		T max = table->data[Y->data[start]*table->dims[0] + X->data[start]];

		for( int i=start+1; i<X->length; i++ ){
			if( max < table->data[Y->data[i]*table->dims[0] + X->data[i]] ) {
				max = table->data[Y->data[i]*table->dims[0] + X->data[i]];
				x = X->data[i];
				y = Y->data[i];
				maxi = i;
			}
		}

		X->data[maxi] = X->data[start];
		Y->data[maxi] = Y->data[start];
		X->data[start] = x;
		Y->data[start] = y;
		s->data[s->length] = max;

		X->length = start + 1;
		Y->length = start + 1;
		s->length++;

		clearArea(table, x, y, threshold, rval);

	}while( len > 1 );


	if( X->length > 0 ){

		I *y_temp = Y->data;
		I *x_temp = X->data;
		X = createVector(X,X->length);
		Y = createVector(Y,Y->length);
		memcpy(X->data, x_temp, X->length*sizeof(T));
		memcpy(Y->data, y_temp, Y->length*sizeof(T));

		for( int i=0; i<X->length; i++ ){

			table->data[Y->data[i]*table->dims[0]+X->data[i]] = s->data[i];
		}

		free(x_temp);
		free(y_temp);
		freeVector(s, true);
	}

	return X->length;
}

} // end of namespace

#endif
