
#ifndef MYREDUCE_H_
#define MYREDUCE_H_

#include <stdio.h>
#include <math.h>

#include "myDataTypes.h"
#include "myLibs.h"

namespace my{

template <typename T>
_Array<T> * reduce(_Array<T> *image){

	if( !image )  return NULL;

	_Array<T> *im = createArray<T>(image->dims[1], (int) floor((image->dims[0]/2)+0.5), image->dims[2]);
	T *s, *d;

	#ifdef OMP_ENABLE
	#pragma omp parallel for
	#endif
	for( int z=0; z<image->dims[2]; z++ ){

		for( int y=0; y<image->dims[1]; y++ ){

			s = image->data + z*image->dims[0]*image->dims[1] + y*image->dims[0];
			d = im->data + z*im->dims[0]*im->dims[1] + y;

			d[0] = (T) (s[0]*0.6875 + s[1]*0.2500 + s[2]*0.0625);

			for ( int x=1; x<im->dims[1]-2; x++ ) {	
				s += 2;
				d += im->dims[0];
				*d = (T) (s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 + s[1]*0.25 + s[2]*0.0625);
			}
			
			s += 2;
			d += im->dims[0];
			if (im->dims[1]*2 <= image->dims[0])  *d = (T) (s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 + s[1]*0.25 + s[2]*0.0625);
			else  *d = (T) (s[1]*0.3125 + s[0]*0.3750 + s[-1]*0.2500 + s[-2]*0.0625);
      
			s += 2;
			d += im->dims[0];
			*d = (T) (s[0]*0.6875 + s[-1]*0.2500 + s[-2]*0.0625);
		}
	}

	image = createArray<T>(im->dims[1], (int) floor((im->dims[0]/2)+0.5), im->dims[2]);

	#ifdef OMP_ENABLE
	#pragma omp parallel for
	#endif
	for( int z=0; z<im->dims[2]; z++ ){

		for( int y=0; y<im->dims[1]; y++ ){

			s = im->data + z*im->dims[0]*im->dims[1] + y*im->dims[0];
			d = image->data + z*image->dims[0]*image->dims[1] + y;
			
			d[0] = (T) (s[0]*0.6875 + s[1]*0.2500 + s[2]*0.0625);
			
			for ( int x=1; x<image->dims[1]-2; x++ ) {	
				s += 2;
				d += image->dims[0];
				*d = (T) (s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 + s[1]*0.25 + s[2]*0.0625);
			}

			s += 2;
			d += image->dims[0];
			if (image->dims[1]*2 <= im->dims[0])  *d = (T) (s[-2]*0.0625 + s[-1]*0.25 + s[0]*0.375 + s[1]*0.25 + s[2]*0.0625);
			else  *d = (T) (s[1]*0.3125 + s[0]*0.3750 + s[-1]*0.2500 + s[-2]*0.0625);
			
			s += 2;
			d += image->dims[0];
			*d = (T) (s[0]*0.6875 + s[-1]*0.2500 + s[-2]*0.0625);
		}
	}

	freeArray(im, true);

	return image;
}

} // end of namespace

#endif
