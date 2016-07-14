#ifndef MYLIBS_H_
#define MYLIBS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>
#include <cmath>

#include "myDataTypes.h"

namespace my{

//-------------------------------------------------------------------------------------------
//----------------------------- GLOBAL FUNCTIONS -----------------------------------
//-----------------------------------------------------------------------------------------

template <typename T>
void myswap(T *x, T *y){

	T temp = *x;
	*x = *y;
	*y = temp;
}

template <typename T>
T mysquare(T x){ return x*x; }

template <typename T>
T mymin(T x, T y){ return (x<=y) ? x : y; }

template <typename T>
T mymax(T x, T y){ return (x>=y) ? x : y; }

template <typename T>
T myround(T x){ return ((x-floor(x)) > 0.5) ? ceil(x) : floor(x); }

template <typename T>
_dataTypes myGetType(){

	if( typeid(T) == typeid(double) ) return _DOUBLE;
	else if( typeid(T) == typeid(float) ) return _FLOAT;
	else if( typeid(T) == typeid(int) ) return _INT;
	else if( typeid(T) == typeid(unsigned char) ) return _UINT8;

	char check[10];
	strncpy(check, typeid(T).name(), ((strlen(typeid(T).name()) < 9 )? strlen(typeid(T).name()) : 9 ) );

	if( strstr(check, "_Vector") ) return _VECTOR;
	else if( strstr(check, "_Array") ) return _ARRAY;

	return _NONE;
}

template <typename T>
char* myGetType(T var){

	return (char*) typeid(T).name();
}

//-------------------------------------------------------------------------------------------
//-------------------------------- CREATE/DELETE ARRAYS - VECTORS -----------------------------
//-------------------------------------------------------------------------------------------

//------------------------------ FREE ARRAY ---------------------------------
template <typename T>
_Array<T> *freeArray(_Array<T> *table, bool ptr){

	if( table ){
	
		if( table->data ) free(table->data);
		table->data = NULL;
		if(ptr) {
			free(table);
			table = NULL;
		}
		else  table->length = table->dims[0] = table->dims[1] = table->dims[2] = 0;
	}
	return table;
}

template <typename T>
_Array<T> *freeArray(_Array<T> *table){

	return freeArray(table, false);
}

//------------------------------ FREE VECTOR ----------------------------------
template <typename T>
_Vector<T> *freeVector(_Vector<T> *vector, bool ptr){

	if( vector ){
		
		if( vector->data ) free(vector->data);
		vector->data = NULL;
		if(ptr) {
			free(vector);
			vector = NULL;
		}
		else vector->length = 0;
	}
	return vector;
}

template <typename T>
_Vector<T> *freeVector(_Vector<T> *table){

	return freeVector(table, false);
}

//------------------------------ FREE ARRAY VECTOR ----------------------------
template <typename T>
_Vector<T> *freeArrayVector(_Vector<T> *table, bool ptr){

	if( table ){

		for( int i=0; i<table->length && table->data; i++ ){  freeArray(&(table->data[i]), false);  }
		free(table->data);
		table->data = NULL;
		if(ptr) {
			free(table);
			table = NULL;
		}
		else table->length = 0;
	}
	return table;
}

template <typename T>
_Vector<T> *freeArrayVector(_Vector<T> *table){

	return freeArrayVector(table, false);
}

template <typename T>
_Vector<T> *freeArrayPointersVector(_Vector<T> *table, bool ptr){

	if( table ){

		for( int i=0; i<table->length && table->data; i++ ){  freeArray(table->data[i], true);  }
		free(table->data);
		table->data = NULL;
		if(ptr) {
			free(table);
			table = NULL;
		}
		else table->length = 0;
	}
	return table;
}

template <typename T>
_Vector<T> *freeArrayPointersVector(_Vector<T> *table){

	return freeArrayPointersVector(table, false);
}

//------------------------------ CREATE ARRAY ----------------------------------

template <typename T>
_Array<T> * newArray(bool init){

	if( init )  return (_Array<T>*) calloc(1, sizeof(_Array<T>));
	return (_Array<T>*) malloc(sizeof(_Array<T>));
}

template <typename T>
_Array<T> * createArray(_Array<T> *table, int x, int y, int z, bool init){

	if( !table )  table = newArray<T>(false);

	table->dims[0] = x;
	table->dims[1] = y;
	table->dims[2] = z;
	table->length = x*y*z;
	
	if( init )  table->data = (T*) calloc( table->length, sizeof(T));
	else  table->data = (T*) malloc( table->length * sizeof(T));

	return table;
}

template <typename T>
_Array<T> * createArray(_Array<T> *table, int x, int y, int z){

	return  createArray((_Array<T>*) table, x, y, z, false);
}

template <typename T>
_Array<T> * createArray(int x, int y, int z){

	return  createArray((_Array<T>*) NULL, x, y, z, false);
}

template <typename T>
_Array<T> * createArray(_Array<T> *table, int* dims, bool init){

	if( dims )  return createArray(table, dims[0], dims[1], dims[2], init);
	else return NULL;
}

template <typename T>
_Array<T> * createArray(_Array<T> *table, int* dims){

	return  createArray(table, dims, false);
}

template <typename T>
_Array<T> * createArray(int* dims){

	return  createArray((_Array<T>*) NULL, dims, false);
}

//------------------------------ CREATE VECTOR --------------------------------
template <typename T>
_Vector<T> * newVector(bool init){

	if( init )  return (_Vector<T>*) calloc(1, sizeof(_Vector<T>));
	return (_Vector<T>*) malloc(sizeof(_Vector<T>));
}

template <typename T>
_Vector<T> * createVector(_Vector<T> *vector, int length, bool init){

	if( !vector )  vector = newVector<T>(init);

	vector->length = length;
	if( init )  vector->data = (T*) calloc(length, sizeof(T));
	else  vector->data = (T*) malloc(length * sizeof(T));

	return vector;
}

template <typename T>
_Vector<T> * createVector(_Vector<T> *vector, int length){

	return  createVector(vector, length, false);
}

template <typename T>
_Vector<T> * createVector(int length, bool init){

	return  createVector<T>(NULL, length, init);
}

template <typename T>
_Vector<T> * createVector(int length){

	return  createVector<T>(NULL, length, false);
}

//---------------------------------------------------------------------------------------
//-------------------------------- ARRAY - VECTOR FUNCTIONS --------------------------------
//-----------------------------------------------------------------------------------------

//------------------------------ COPY ARRAY -----------------------------------
template <typename T>
_Array<T> *copyArray(_Array<T> *table, _Array<T>* ctable){

	if( !table )  return NULL;

	ctable = createArray(ctable, table->dims);
	memcpy(ctable->data, table->data, table->length*sizeof(T));

	return ctable;
}

template <typename T>
_Array<T> *copyArray(_Array<T> *table){

	return  copyArray(table, (_Array<T>*)NULL);
}

//------------------------------  COPY VECTOR ----------------------------------
template <typename T>
_Vector<T> * copyVector(_Vector<T> *table, _Vector<T>* ctable){

	if( !table )  return NULL;
	
	ctable = createVector(ctable, table->length);
	memcpy(ctable->data, table->data, table->length*sizeof(T));

	return ctable;
}

template <typename T>
_Vector<T> *copyVector(_Vector<T> *table){

	return  copyVector(table, (_Vector<T>*) NULL);
}


//------------------------------ VECTOR MIN\MAX ------------------------------
template <typename T>
T vectormin(T *table, int length){

	if( !table || length < 1 ) return 0;
	
	T min = table[0];
	for( int i=1; i<length; i++ ){  if( min > table[i] ) min = table[i];  }

	return min;
}

template <typename T>
T vectormax(T *table, int length){

	if( !table || length < 1 ) return 0;
	
	T max = table[0];
	for( int i=1; i<length; i++ ){  if( max < table[i] ) max = table[i];}

	return max;
}

//----------------------- ARRAY MIN\MAX --------------------------------------
template <typename T>
T arraymin(T *table, int length, int start, int step){

	if( !table || length < 1 ) return 0;
	if( start >= length || start < 0 ) return 0;
	if( step >= length || step < 1 ) return 0;

	T min = table[start];
	for( int i=start+step; i<length; i+=step ){  if( min > table[i] ) min = table[i];
	}

	return min;
}

template <typename T>
T arraymax(T *table, int length, int start, int step){

	if( !table || length < 1 ) return 0;
	if( start >= length || start < 0 ) return 0;
	if( step >= length || step < 1 ) return 0;

	T max = table[start];
	for( int i=start+step; i<length; i+=step ){  if( max < table[i] ) max = table[i];  }

	return max;
}

//------------------------------ VECTOR DELETE ITEM ---------------------------
template <typename T>
int vectordelete(_Vector<T> *vector, int index){

	if( !vector || index < 0 || index >= vector->length ) return -1;

	for( int i=index; i<vector->length-2; i++ ){  vector->data[i] = vector->data[i+1];  }
	vector->length--;

	return index;
}

//------------------------------ ARRAY ADDITION ------------------------------
template <typename T>
_Array<T> *matrixAddition(_Array<T> *A, _Array<T> *B, _Array<T> *C) {

	if( !A || !B ) return NULL;
	if( A->length != B->length ) return NULL;
	if( !C ) C = createArray(C, A->dims);

	for( int i=0; i<C->length; i++ ){ C->data[i] = A->data[i] + B->data[i]; }

	return C;
}

template <typename T>
_Array<T> * matrixAddition(_Array<T> *A, _Array<T> *B) {

	return matrixAddition(A, B, NULL);
}

//------------------------------ ARRAY INCREASE ---------------------------------
template <typename T>
_Array<T> * matrixIncrease(_Array<T> *A, T B, _Array<T> *C) {

	if( !A ) return NULL;
	if( !C ) C = createArray<T>(A->dims);

	for( int i=0; i<C->length; i++ ){ C->data[i] = A->data[i] + B; }

	return C;
}

template <typename T>
_Array<T> * matrixIncrease(_Array<T> *A, T B) {

	return  matrixIncrease(A, B, (_Array<T>*) NULL);
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------- DATA STRUCTURES ------------------------------
//--------------------------------------------------------------------------------------------

//----------------------------- FEATURE PYRAMID -------------------------
template <typename T>
_featurePyramid<T> * newFeaturePyramid(bool init){

	if( init )  return (_featurePyramid<T>*) calloc(1, sizeof(_featurePyramid<T>));
	return (_featurePyramid<T>*) malloc(sizeof(_featurePyramid<T>));
}

template <typename T>
void freeFeaturePyramid(_featurePyramid<T> *pyra, bool ptr){

	if( pyra ){

		for( int i=0; i<pyra->feat.length; i++ ) pyra->feat.data[i] = freeArray(pyra->feat.data[i], true);
		freeVector(&pyra->feat, false);
		freeVector(&pyra->scale, false);
		if(ptr){
			free(pyra);
			pyra = NULL;
		}
		else memset(pyra, 0, sizeof(_featurePyramid<T>));
	}
}

//----------------------------- PART -------------------------------------
template <typename T>
_part<T> * newPart(bool init){

	if( init )  return (_part<T>*) calloc(1, sizeof(_part<T>));
	return (_part<T>*) malloc(sizeof(_part<T>));
}

template <typename T>
void freePart(_part<T> *part, bool ptr){

	if( part ){

		freeVector(&part->w, false);
		freeArray(part->score, true);
		freeArray(part->Ix, true);
		freeArray(part->Iy, true);
		if( ptr ){
			free(part);
			part = NULL;
		}
		else memset(part, 0, sizeof(_part<T>));
	}
}

template <typename T>
void freeParts(_Vector< _part<T> > *parts, bool ptr){

	if( parts ){
		
		if( parts->data ) for( int i=0; i<parts->length; i++ ){  freePart(&parts->data[i], false);  }
		free(parts->data);
		parts->data = NULL;
		if( ptr ){
			free(parts);
			parts = NULL;
		}
		else memset(parts, 0, sizeof(_Vector< _part<T> >));
	}
}

//---------------------------- COMPONENTS -------------------------------
template <typename T>
void freeComponent(_Vector< _part<T> > *component, bool ptr){

	freeParts(component, ptr);
}

template <typename T>
void freeComponents(_Vector< _Vector< _part<T> > > *components, bool ptr){

	if( components ){

		if( components->data ) for( int i=0; i<components->length; i++ ){  freeComponent(&components->data[i], false);  }
		free(components->data);
		components->data = NULL;
		if( ptr ) {
			free(components);
			components = NULL;
		}
		else memset(components, 0, sizeof(_Vector< _Vector< _part<T> > >));
	}
}

//----------------------------- DEFS ------------------------------------
template <typename T>
_def<T> * newDef(bool init){

	if( init )  return (_def<T>*) calloc(1, sizeof(_def<T>));
	return (_def<T>*) malloc(sizeof(_def<T>));
}

template <typename T>
void freeDef(_def<T>* def, bool ptr){

	if( def ){

		freeVector(&def->anchor, false);
		freeVector(&def->w, false);
		if( ptr ){
			free(def);
			def = NULL;
		}
		else memset(def, 0, sizeof(_def<T>));
	}
}

template <typename T>
void freeDefs(_Vector< _def<T> >* defs, bool ptr){

	if( defs ){

		if( defs->data ) for( int i=0; i<defs->length; i++ ){  freeDef(&defs->data[i], false);  }
		free(defs->data);
		defs->data = NULL;
		if( ptr ){
			free(defs);
			defs = NULL;
		}
		else memset(defs, 0, sizeof(_Vector< _def<T> >));
	}
}


//----------------------------- FILTERS ---------------------------------
template <typename T>
_filter<T> * newFilter(bool init){

	if( init )  return (_filter<T>*) calloc(1,sizeof(_filter<T>));
	return (_filter<T>*) malloc(sizeof(_filter<T>));
}

template <typename T>
void freeFilter(_filter<T> *filter, bool ptr){

	if( filter ){

		freeArray(&filter->w, false);
		if( ptr ){
			free(filter);
			filter = NULL;
		}
		else memset(filter, 0, sizeof(_filter<T>));
	}
}

template <typename T>
void freeFilters(_Vector< _filter<T> > *filters, bool ptr){

	if( filters ){

		if( filters->data ) for( int i=0; i<filters->length; i++ ) {  freeFilter(&filters->data[i], false);  }
		free(filters->data);
		filters->data = NULL;
		if(ptr){
			free(filters);
			filters = NULL;
		}
		else memset(filters, 0, sizeof(_Vector< _filter<T> >));
	}
}

//----------------------------- MODEL ------------------------------------
template <typename T>
_model<T> * newModel(bool init){

	if( init )  return (_model<T>*) calloc(1, sizeof(_model<T>));
	return (_model<T>*) malloc(sizeof(_model<T>));
}

template <typename T>
void freeModel(_model<T> *model, bool ptr){

	if( model ){

		freeVector(&model->maxsize, false);
		freeComponents(&model->components, false);
		freeDefs(&model->defs, false);
		freeFilters(&model->filters, false);
		if( ptr ){
			free(model);
			model = NULL;
		}
		else memset(model, 0, sizeof(_model<T>));
	}
}

//----------------------------- BOX -------------------------------------
template <typename T>
_box<T> *newBox(bool init){

	if( init )  return (_box<T>*) calloc(1, sizeof(_box<T>));
	return  (_box<T>*) malloc(sizeof(_box<T>));
}

template <typename T>
void freeBox(_box<T> *box, bool ptr){

	if( box ){

		freeArray(box->xy, true);
		if( ptr ){
			free(box);
			box = NULL;
		}
		else memset(box, 0, sizeof(_box<T>));
	}
}

template <typename T>
void freeBoxes(_Vector< _box<T> > *boxes, bool ptr){

	if( boxes ){

		if( boxes->data ) for( int i=0; i<boxes->length; i++ ){  freeBox(&(boxes->data[i]), false);  }
		free(boxes->data);
		boxes->data = NULL;
		if( ptr ) {
			free(boxes);
			boxes = NULL;
		}
		else memset(boxes, 0, sizeof(_Vector< _box<T> >));
	}
}

}	/* end of namespace my	*/

#endif	/* MYLIBS_H_	*/


