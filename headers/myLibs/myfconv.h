#ifndef MYFCONV_H_
#define MYFCONV_H_

#include <stdio.h>
#include <string.h>

#include "myDataTypes.h"
#include "myLibs.h"

namespace my{

//--------------------------------------------- CONVOLVE -----------------------------------

template <typename T>
_Array<T> * convolve(_Array<T> *response, _Array<T> *feat, _Array<T> *filter) {

	if( !feat || !filter || !feat->data || !filter->data ) return NULL;
	
	T *feat_ptr, *filter_ptr, *resp_ptr;
	T val;

	if( !response ) response = createArray(response, (feat->dims[0] - filter->dims[0] + 1), (feat->dims[1] - filter->dims[1] + 1), 1, true);

	for( int f=0; f<feat->dims[2]; f++ ){

		resp_ptr = response->data;
		feat_ptr = feat->data + (f * feat->dims[0] * feat->dims[1]);
		filter_ptr = filter->data + (f * filter->dims[0] * filter->dims[1]);
		
		for( int y=0; y<response->dims[1]; y++ ){

			for( int x=0; x<response->dims[0]; x++ ){

				val = 0;
				T *feat_tmp = feat_ptr + y*feat->dims[0] + x;
				T *filter_tmp = filter_ptr;
				
				for( int xp=0; xp<5/*filter->dims[0]*/; xp++ ){
					
					for (int yp=0; yp<5/*filter->dims[1]*/; yp++) {  val += feat_tmp[yp] * filter_tmp[yp];  }
					feat_tmp += feat->dims[0];
					filter_tmp += filter->dims[0];
				}
				*resp_ptr += val;
				resp_ptr++;
			}
		}
	}
	return response;
}

template <typename T>
_Array<T> * convolve(_Array<T> *feat, _Array<T> *filter) {

	return convolve((_Array<T>*) NULL, feat, filter);
}

//------------------------------- FEATURE_RESPONCE --------------------------------------------

template <typename T>
_Vector< _Array<T>* > *featureResponce(_Vector< _Array<T>* > *responce, _Array<T> *feat, _Vector< _filter<T> > *filters){

	if( !filters || !filters->data ) return NULL;
	
	responce = createVector(responce, filters->length, true);

	#ifdef OMP_ENABLE
	#pragma omp parallel for
	#endif
	for( int i=0; i<filters->length; i++ ){

		responce->data[i] = convolve(feat, &(filters->data[i].w));
	}

	return responce;
}

template <typename T>
_Vector< _Array<T>* > *featureResponce(_Array<T> *feat, _Vector< _filter<T> > *filters){

	return featureResponce((_Vector< _Array<T>* >*) NULL, feat, filters);
}

//------------------------------- FEATURES RESPONCES ------------------------------------------

template <typename T>
_Vector< _Vector< _Array<T>* >* > *featureResponces(_Vector< _Vector< _Array<T>* >* > *responces, _Vector< _Array<T> > *features, _Vector< _filter<T> > *filters, int start, int end){

	if( !features || !features->data ) return NULL;

	if( start < 0 || start > end ) start = 0;
	if( end < start || end > features->length ) end = features->length;
	if( !responces ) responces = createVector(responces, features->length, true);

	for( int i=start; i<end; i++ ){

		responces->data[i] = featureResponce(&(features->data[i]), filters);
	}

	return responces;
}

template <typename T>
_Vector< _Vector< _Array<T>* >* > *featureResponces(_Vector< _Array<T> > *features, _Vector< _filter<T> > *filters, int start, int end){

	return featureResponces((_Vector< _Vector< _Array<T>* >* >*)NULL, features, filters, start, end);
}

} // end of namespace

#endif
