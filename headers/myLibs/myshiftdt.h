
#ifndef MYSHIFTDT_H_
#define MYSHIFTDT_H_

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cmath>
#include <limits.h>

#include "myDataTypes.h"
#include "myLibs.h"

namespace my{

//---------------------------------------------------------------------------------------------
//--------------------------------- DISTANCE TRANSFORMATION V1.3 ------------------------------
//---------------------------------------------------------------------------------------------
template <typename T, typename I>
_Array<T> * distanceTransformation(_Array<T> *transform, _Array<T> *score, T *w, int startx, int starty, int step, _Array<I> *Ix, _Array<I> *Iy) {

	if( !score || !Ix || !Iy || !w )  return NULL;
	
	if( !transform ) transform = createArray(transform, score->dims);
	Ix = createArray(Ix, score->dims);
	Iy = createArray(Iy, score->dims);

	T *temp = (T*) malloc(transform->length * sizeof(T));
	I *tempIy = (int*) malloc(Iy->length * sizeof(int));
	int lenx = score->dims[0];
	int leny = score->dims[1];
	I *v = (int*) ( (leny > lenx )? malloc(leny * sizeof(int)) : malloc(lenx * sizeof(int)) );		//Same mem for X,Y
	float *z = (float*) ( (leny > lenx )? malloc((leny + 1) * sizeof(float)) : malloc((lenx + 1) * sizeof(float)) );	//Same mem for X,Y

	T a, b;
	T *_score, *_transform;
	I *_ix, *_iy;

	//1D distance transform for X
	a = -w[2];
	b = -w[3];
	_score = score->data;
	_transform = temp;
	_iy = tempIy;

	for( int x=1; x<=lenx; x++ ) {

		v[0] = 0;
		z[0] = (float) INT_MIN;
		z[1] = (float) INT_MAX;

		for( int q=1, k=0; q<leny; q++, k++ ){

			float s = (float)(  ((_score[q*lenx] - _score[v[k]*lenx]) - b*(q - v[k]) + a*(mysquare(q) - mysquare(v[k]))) / (2*a*(q-v[k]))  );
			while (s <= z[k]) {
				k--;
				s = (float)(  ((_score[q*lenx] - _score[v[k]*lenx]) - b*(q - v[k]) + a*(mysquare(q) - mysquare(v[k]))) / (2*a*(q-v[k]))  );
			}
			v[k+1] = q;
			z[k+1] = s;
			z[k+2] = (float) INT_MAX;
		}

		for (int i=0, k=0, q=starty-1; i<leny; i++, q+=step) {
			
			while (z[k+1] < q) { k++; }
			*_transform = (T) (a*mysquare(q-v[k]) + b*(q-v[k]) + _score[v[k]*lenx]);
			*_iy = v[k];
			
			_transform += lenx;
			_iy += lenx;
		}

		_score++;
		_transform = temp + x;
		_iy = tempIy + x;
	}

	//1D distance transform for Y
	a = -w[0];
	b = -w[1];
	_score = temp;
	_transform = transform->data;
	_ix = Ix->data;

	for( int y=0; y<leny; y++ ) {

		v[0] = 0;
		z[0] = (float) INT_MIN;
		z[1] = (float) INT_MAX;

		for( int q=1, k=0; q<lenx; q++, k++ ){

			float s = (float)(  ((_score[q] - _score[v[k]]) - b*(q - v[k]) + a*(mysquare(q) - mysquare(v[k]))) / (2*a*(q-v[k]))  );
			while (s <= z[k]) {
				k--;
				s = (float)(  ((_score[q] - _score[v[k]]) - b*(q - v[k]) + a*(mysquare(q) - mysquare(v[k]))) / (2*a*(q-v[k]))  );
			}
			v[k+1] = q;
			z[k+1] = s;
			z[k+2] = (float) INT_MAX;
		}

		for (int i=0, k=0, q=startx-1; i<lenx; i++, q+=step) {
			
			while (z[k+1] < q) { k++; }
			*_transform = (T) (a*mysquare(q-v[k]) + b*(q-v[k]) + _score[v[k]]);
			*_ix = v[k];

			_transform++;
			_ix++;
		}

		_score += lenx;
	}

	free(v);
	free(z);

	//Fix Ix, Iy values
	_ix = Ix->data;
	_iy = Iy->data;
	v = tempIy;
	for (int y = 0; y<leny; y++, v+=lenx) {

		for (int x = 0; x<lenx; x++, _iy++, _ix++) {

			*_iy = v[*_ix] + 1;
			(*_ix)++;
		}
	}

	free(tempIy);
	free(temp);

	return transform;
}

template <typename T, typename I, typename W>
_Array<T> * distanceTransformation(_Array<T> *score, W *w, int startx, int starty, int step, _Array<I> *Ix, _Array<I> *Iy) {

	return distanceTransformation((_Array<T>*) NULL, score, w, startx, starty, step, Ix, Iy);
}

//---------------------------------------------------------------------------------------------
//--------------------------------- DISTANCE TRANSFORMATION ORIGINAL V1.3 ---------------------
//---------------------------------------------------------------------------------------------
template <typename T, typename T2>
T * transformation(T *transform, T *score, int *ixy, int len, int ptr_step, T2 a, T2 b, int start, int step, float *z, int *v){

	if( !score || !transform )  return NULL;
	
	T *_trans = transform;
	int k, q;

	k = 0;
	q = 0;
	v[0] = 0;
	z[0] = (float) INT_MIN;		//Like +-INF
	z[1] = (float) INT_MAX;	

	for( q=1; q<len; q++ ){

		float s = (float)(  ((score[q*ptr_step] - score[v[k]*ptr_step]) - b*(q - v[k]) + a*(mysquare(q) - mysquare(v[k]))) / (2*a*(q-v[k]))  );
		while (s <= z[k]) {
			k--;
			s = (float)(  ((score[q*ptr_step] - score[v[k]*ptr_step]) - b*(q - v[k]) + a*(mysquare(q) - mysquare(v[k]))) / (2*a*(q-v[k]))  );
		}
		k++;
		v[k] = q;
		z[k] = s;
		z[k+1] = (float) INT_MAX;	//+INF
	}

	k = 0;
	q = start-1;

	for (int i=0; i<len; i++) {
			
		while (z[k+1] < q) { k++; }
		*_trans = (T) a*mysquare(q-v[k]) + b*(q-v[k]) + score[v[k]*ptr_step];
		*ixy = v[k];

		q += step;
		_trans += ptr_step;
		ixy += ptr_step;
	}
	
	return transform;
}

template <typename T, typename I, typename W>
_Array<T> * dTransformation(_Array<T> *transform, _Array<T> *score, W *w, int startx, int starty, int step, _Array<I> *Ix, _Array<I> *Iy) {

	if( !score || !Ix || !Iy )  return NULL;
	
	if( !transform ) transform = createArray(transform, score->dims);
	Ix = createArray(Ix, score->dims);
	Iy = createArray(Iy, score->dims);

	T *tempTransform = (T*) malloc(transform->length * sizeof(T));
	int *tempIy = (int*) malloc(Iy->length * sizeof(int));

	int *v = (int*) ( (score->dims[1] > score->dims[0]) ? malloc(score->dims[1] * sizeof(int)) : malloc(score->dims[0] * sizeof(int)) );
	float *z = (float*) ( (score->dims[1] > score->dims[0]) ? malloc((score->dims[1]+1) * sizeof(float)) : malloc((score->dims[0]+1) * sizeof(float)) );

	for( int x=0; x<score->dims[0]; x++ ){

		transformation(tempTransform + x, score->data + x, tempIy + x, score->dims[1], score->dims[0], -w[2], -w[3], starty, step, z, v);
	}

	for( int y=0; y<score->length; y+=score->dims[0] ){

		transformation(transform->data + y, tempTransform + y, Ix->data + y, score->dims[0], 1, -w[0], -w[1], startx, step, z, v);
	}

	//Fix Ix, Iy values
	for (int y=0; y<score->length; y+=score->dims[0]) {

		int i;
		for (int x=0; x<score->dims[0]; x++) {

			i = x+y;
			Iy->data[i] = tempIy[Ix->data[i] + y] + 1;
			Ix->data[i]++;
		}
	}

	free(v);
	free(z);
	free(tempTransform);
	free(tempIy);

	return transform;
}

} // end of namespace

#endif
