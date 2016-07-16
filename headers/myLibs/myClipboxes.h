#ifndef MYCLIPBOXES_H_
#define MYCLIPBOXES_H_

#include <stdio.h>
#include <string.h>

#include "myDataTypes.h"

namespace my{

template <typename T, typename I>
_Vector< _box<T> > *clipboxes(_Vector< _box<T> > *boxes, _Array<I> *image){

	if( !boxes || !image ) return NULL;

	for( int i=0; i<boxes->length; i++ ){

		for( T *ptr=boxes->data[i].xy->data; ptr<(boxes->data[i].xy->data + boxes->data[i].xy->length); ptr+=4 ){

			ptr[0] = (T) mymax(ptr[0], (T)1);
			ptr[1] = (T) mymax(ptr[1], (T)1);
			ptr[2] = (T) mymin(ptr[2], (T)image->dims[0]);
			ptr[3] = (T) mymin(ptr[3], (T)image->dims[1]);
		}
	}
	return boxes;
}

} // end of namespace

#endif
