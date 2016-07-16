#ifndef MYRESIZE_H_
#define MYRESIZE_H_

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "myDataTypes.h"
#include "myLibs.h"

namespace my{

template <typename T>
struct alphainfo {
	int si;
	int di;
	T alpha;
};


template <typename T, typename F>
void alphacopy(T *src, T *dst, alphainfo<F> *ofs, int n, int l) {
	
	if( !src || !dst || !ofs ) return;
	
	alphainfo<F> *end = ofs + n;

	for( ofs; ofs!=end ; ofs++ ){

		dst[ofs->di] += (T) (ofs->alpha * src[ofs->si]);
	}
}

//---------------------------------------------------------------------------------------------
//---------------------------------------------RESIZE ----------------------------------------
//---------------------------------------------------------------------------------------------

template <typename T, typename F>
_Array<T> *resize(_Array<T> *image, F scale){

	if( !image ) return NULL;
	if( scale >= 1 ) return copyArray(image);

	_Array<T> *im = createArray<T>((_Array<T>*)NULL, image->dims[1], (int)myround(image->dims[0]*scale), image->dims[2], true);
	F sc = (F)im->dims[1] / (F)image->dims[0];
	F invscale = (F)1 / (F)sc;
	int len = (int) ceil(im->dims[1] * invscale) + 2*im->dims[1];
	int k=0;
	alphainfo<F> *ofs = (alphainfo<F>*) malloc(len * sizeof(alphainfo<F>));

	#ifdef OMP_ENABLE
	#pragma omp parallel for
	#endif
	for( int dy=0; dy<im->dims[1]; dy++ ){

		F fsy1 = dy * invscale;
		F fsy2 = fsy1 + invscale;
		int sy1 = (int) ceil(fsy1);
		int sy2 = (int) floor(fsy2);       

		if (sy1 - fsy1 > 0.001) { 

			assert(k < len);
			assert(sy1-1 >= 0); 
			ofs[k].di = dy * im->dims[0];
			ofs[k].si = sy1 - 1 ;
			ofs[k++].alpha = (sy1 - fsy1) * sc;
		}

		for (int sy=sy1; sy<sy2; sy++) {

			assert(k < len);
			assert(sy < image->dims[0]);
			ofs[k].di = dy * im->dims[0];
			ofs[k].si = sy;
			ofs[k++].alpha = sc;
		}

		if (fsy2 - sy2 > 0.001) {

			assert(k < len);
			assert(sy2 < image->dims[0]);
			ofs[k].di = dy * im->dims[0];
			ofs[k].si = sy2;
			ofs[k++].alpha = (fsy2 - sy2) * sc;
		}
	}

	for ( int c=0; c<image->dims[2]; c++ ) {
		for ( int x=0; x<image->dims[1]; x++ ) {

			T *s = image->data + c*image->dims[0]*image->dims[1] + x*image->dims[0];
			T *d = im->data + c*im->dims[0]*im->dims[1] + x;
			alphacopy(s, d, ofs, k, im->length);
		}
	}	
	
	free(ofs);

	image = createArray<T>((_Array<T>*)NULL, im->dims[1], (int)myround(im->dims[0]*scale), im->dims[2], true);
	sc = (F)image->dims[1] / (F)im->dims[0];
	invscale = (F)1 / (F)sc;
	len = (int) ceil(image->dims[1]*invscale) + 2*image->dims[1];
	ofs = (alphainfo<F>*) malloc(len * sizeof(alphainfo<F>));
	k = 0;

	#ifdef OMP_ENABLE
	#pragma omp parallel for
	#endif
	for( int dy=0; dy<image->dims[1]; dy++ ){

		F fsy1 = dy * invscale;
		F fsy2 = fsy1 + invscale;
		int sy1 = (int) ceil(fsy1);
		int sy2 = (int) floor(fsy2);       

		if (sy1 - fsy1 > 0.001) { 

			assert(k < len);
			assert(sy1-1 >= 0); 
			ofs[k].di = dy * image->dims[0];
			ofs[k].si = sy1 - 1 ;
			ofs[k++].alpha = (sy1 - fsy1) * sc;
		}

		for (int sy=sy1; sy<sy2; sy++) {

			assert(k < len);
			assert(sy < im->dims[0]);
			ofs[k].di = dy * image->dims[0];
			ofs[k].si = sy;
			ofs[k++].alpha = sc;
		}

		if (fsy2 - sy2 > 0.001) {

			assert(k < len);
			assert(sy2 < im->dims[0]);
			ofs[k].di = dy * image->dims[0];
			ofs[k].si = sy2;
			ofs[k++].alpha = (fsy2 - sy2) * sc;
		}
	}

	for ( int c=0; c<im->dims[2]; c++ ) {
		for ( int x=0; x<im->dims[1]; x++ ) {

			T *s = im->data + c*im->dims[0]*im->dims[1] + x*im->dims[0];
			T *d = image->data + c*image->dims[0]*image->dims[1] + x;
			alphacopy(s, d, ofs, k, image->length);
		}
	}

	free(ofs);
	freeArray(im, true);
	
	return image;
}

} // end of namespace

#endif
