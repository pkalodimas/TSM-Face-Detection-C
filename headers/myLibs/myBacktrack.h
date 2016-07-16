#ifndef MYBACKTRACK_H_
#define MYBACKTRACK_H_

#include <stdio.h>

#include "myDataTypes.h"
#include "myLibs.h"

using namespace my;

namespace my{
	
//--------------------------------------------- BOX MEM -------------------------------------

template <typename T>
_Array<T> * membox(_Array<T> *box){

	if( !box ) return NULL;

	_Array<T> *mbox = createArray<T>(box->dims[1], box->dims[2], box->dims[0]);
	int i = 0;

	for( int x=0; x<box->dims[2]; x++ ){
		
		for( int y=0; y<box->dims[1]; y++){

			for( int z=0; z<box->dims[0]; z++){

				mbox->data[z*box->dims[2]*box->dims[1] + y + x*box->dims[1]] = box->data[i];
				i++;
			}
		}
	}

	return mbox;
};


//--------------------------------------------- SUB2IND -------------------------------------

template <typename T, typename I>
_Vector<I> *sub2ind(_Vector<I> *indexes, int dimX, _Vector<T> *x, _Vector<T> *y){

	if( !x || !y || !x->data || !y->data ) return NULL;

	if( !indexes || indexes->length < x->length) indexes = createVector(indexes, x->length);

	for( int i=0; i<indexes->length; i++ ){

		indexes->data[i] = (I) ((y->data[i]-1)*dimX + (x->data[i]-1));
	}

	return indexes;
};

template <typename T, typename I>
_Vector<I> *sub2ind(int dimX, _Vector<T> *x, _Vector<T> *y){

	return sub2ind((_Vector<I>*) NULL, dimX, x, y);
}

//------------------------------------- BACKTRACK ------------------------------------

template <typename T, typename I>
_Vector< _Array<T>* > * backtrack(_Vector<I> *X, _Vector<I> *Y, _Vector< _part<T> > *parts, _featurePyramid<T> *pyra){

	if( !X || !Y || !X->data || ! Y->data || !parts || !parts->data || !pyra ) return NULL;

	int numparts = parts->length;
	_Array<T> *ptr = createArray<T>(X->length, 2, numparts);
	_Vector< _Array<T>* > *box = createVector< _Array<T>* >(X->length);
	_Vector<T> *x = createVector<T>(X->length);
	_Vector<T> *y = createVector<T>(Y->length);
	_Vector<int> *inds = createVector<int>(x->length);
	_part<T> *p;
	int k, l;
	int padx = pyra->padx;
	int pady = pyra->pady;
	float scale;
	T *x_ptr;

	k = 0;
	l = k*4;
	p = &(parts->data[k]);
	scale = (float) pyra->scale.data[p->level-1];

	for( int i=0; i<X->length; i++ ){
		x->data[i] = (T) X->data[i] + 1;
		y->data[i] = (T) Y->data[i] + 1;
	}

	for( int i=0; i<box->length; i++ ){
		box->data[i] = (_Array<T>*) createArray<T>(4,numparts,1);
	}

	x_ptr = ptr->data + k*ptr->dims[0]*ptr->dims[1];
	for( int i=0, j=0; i<x->length; i++, j+=2 ){
		x_ptr[j] = x->data[i];
		x_ptr[j+1] = y->data[i];
	}

	for( int i=0; i<x->length; i++ ){
		box->data[i]->data[l] = (x->data[i]-1-padx)*scale + 1;
		box->data[i]->data[l+1] = (y->data[i]-1-pady)*scale + 1;
		box->data[i]->data[l+2] = box->data[i]->data[l] + (p->sizx)*scale - 1;
		box->data[i]->data[l+3] = box->data[i]->data[l+1] + (p->sizy)*scale - 1;
	}

	for( k=1, l=k*4; k<numparts; k++, l+=4 ){		//numparts

		p = &(parts->data[k]);
		int par = p->parent - 1;

		x_ptr = ptr->data + par*ptr->dims[0]*ptr->dims[1];
		for( int i=0, j=0; i<x->length; i++, j+=2 ){
			x->data[i] = x_ptr[j];
			y->data[i] = x_ptr[j+1];
		}

		inds = sub2ind<T,int>(inds, p->Ix->dims[0], x, y);

		x_ptr = ptr->data + k*ptr->dims[0]*ptr->dims[1];
		for( int i=0, j=0; i<inds->length; i++, j+=2 ){
			x_ptr[j] = p->Ix->data[(int)inds->data[i]];
			x_ptr[j+1] = p->Iy->data[(int)inds->data[i]];
		};

		scale = (float) pyra->scale.data[p->level-1];

		x_ptr = ptr->data + k*ptr->dims[0]*ptr->dims[1];
		for( int i=0, j=0; i<ptr->dims[0]; i++, j+=2 ){
			box->data[i]->data[l] = (x_ptr[j] - 1 - padx) * scale + 1;
			box->data[i]->data[l+1] = (x_ptr[j+1] - 1 - pady) * scale + 1;
			box->data[i]->data[l+2] = box->data[i]->data[l] + p->sizx * scale - 1;
			box->data[i]->data[l+3] = box->data[i]->data[l+1] + p->sizy * scale - 1;
		}
	}

	freeVector(inds, true);
	freeVector(x, true);
	freeVector(y, true);
	freeArray(ptr, true);

	return box;
};

} // end of namespace

#endif
