#ifndef MYDATAPRINT_H_
#define MYDATAPRINT_H_

#include <stdio.h>
#include "myDataTypes.h"
#include "myLibs.h"

namespace my{
	
//-------------------------------------------------------------------------------------------
//-------------------------------- GLOBAL DATA -----------------------------------------
//-----------------------------------------------------------------------------------------

//----------------------------- PRINT ARRAY ------------------------------------------------
template <typename T>
void printArray(_Array<T>* table, bool data){

	if( !table ) return;
		
	printf("(%dx%dx%d Array) ", table->dims[0], table->dims[1], table->dims[2]);
	if( data ) {

		char format[10];
		switch (myGetType<T>())
		{
		case _INT:{
			sprintf(format, "%%d, ");
			break;
					}
		case _INT64:{
			sprintf(format, "%%d, ");
			break;
					}
		case _UINT8:{
			sprintf(format, "%%u, ");
			break;
					}
		default:{
			sprintf(format, "%%f, ");
			break;
				}
		}

		for( int z=0; z<table->dims[2]; z++ ){
			printf("\n-----Tile %d:", z);
			for( int y=0; y<table->dims[1]; y++ ){
				printf("\n-----Line %d:\n", y);
				for( int x=0; x<table->dims[0]; x++ ){
					printf(format, table->data[z*table->dims[0]*table->dims[1] + y*table->dims[0] + x]);
				}
			}
		}
	}
	printf("\n");
}

//----------------------------- PRINT VECTOR -----------------------------------------------
template <typename T>
void printVector(_Vector<T> *table, bool data){

	if( !table ) return;

	printf("(%dx1 Vector) ", table->length);

	if( data ) {

		char format[10];
		switch (myGetType<T>())
		{
		case _INT:{
			sprintf(format, "%%d, ");
			break;
					}
		case _INT64:{
			sprintf(format, "%%d, ");
			break;
					}
		case _UINT8:{
			sprintf(format, "%%u, ");
			break;
					}
		default:{
			sprintf(format, "%%f, ");
			break;
				}
		}

		for( int i=0; i<table->length; i++ ){  printf(format, table->data[i]);  }
	}
}

//----------------------------- PRINT ARRAY VECTOR -----------------------------------------
template <typename T>
void printArrayVector(_Vector< _Array<T> > *table, bool data){

	if( !table ) return;

	printf("(%dx1 Array Vector)\n", table->length);
	for( int i=0; i<table->length; i++ ){
		printf("Array %d---------------", i);
		printArray(&table->data[i], data);
	}
}

//-------------------------------------------------------------------------------------------
//-------------------------------- DATA STRUCTURES ---------------------------------------
//-----------------------------------------------------------------------------------------

//----------------------------- PRINT FEATURE PYRAMID ---------------------------------------
template <typename T>
void printFeaturePyramid(_featurePyramid<T> *pyra, bool *data){

	if( pyra ){

		printf("interval=%d, ", pyra->interval);
		printf("imx=%d, ", pyra->imx);
		printf("imy=%d, ", pyra->imy);
		printf("padx=%d, ", pyra->padx);
		printf("pady=%d, ", pyra->pady);
		printf("scale=");
		printVector(&pyra->scale, data[0]);
		printf("\nFeatures:");
		printArrayVector(&pyra->feat, data[1]);
	}
}


//----------------------------- PRINT PART --------------------------------------------------
template <typename T>
void printPart(_part<T> *part, bool *data){

	if( part ){

		printf("defid=%d, ", part->defid);
		printf("filterid=%d, ", part->filterid);
		printf("parent=%d, ", part->parent);
		printf("sizx=%d, ", part->sizx);
		printf("sizy=%d, ", part->sizy);
		printf("filterI=%d, ", part->filterI);
		printf("defI=%d, ", part->defI);
		printf("scale=%d, ", part->scale);
		printf("startx=%d, ", part->startx);
		printf("starty=%d, ", part->starty);
		printf("step=%d, ", part->step);
		printf("level=%d\n", part->level);
		printf("w=");
		printVector(&part->w, data[0]);
		printf("\nIx=");
		printArray(part->Ix, data[1]);
		printf("\nIy=");
		printArray(part->Iy, data[2]);
		printf("\nscore=");
		printArray(part->score, data[3]);
	}
}

//----------------------------- PRINT PARTS -------------------------------------------------
template <typename T>
void printParts(_Vector< _part<T> > *parts, bool *data){

	if( parts ){

		for( int i=0; i<parts->length; i++ ){

			printf("Part %d----------------------------------\n", i);
			printPart(&parts->data[i], data);
		}
	}
}

//-----------------------------PRINT COMPONENT ---------------------------------------------
template <typename T>
void printComponent(_Vector< _part<T> > *component, bool *data){

	printParts(component, data);
}

//-----------------------------PRINT COMPONENTS ---------------------------------------------
template <typename T>
void printComponents(_Vector< _Vector< _part<T> > > *components, bool *data){

	if( components ){

		printf("\n");
		for( int i=0; i<components->length; i++ ){

			printf("Component %d----------------------\n", i);
			printParts(&components->data[i], data);
		}
	}
}

//----------------------------- PRINT DEF --------------------------------------------------
template <typename T>
void printDef(_def<T>* def, bool *data){

	if( def ){

		printf("i=%d, ", def->i);
		printf("w=");
		printVector(&def->w, data[0]);
		printf(", ");
		printf("anchor=");
		printVector(&def->anchor, data[1]);	
	}
}

//----------------------------- PRINT DEFS -------------------------------------------------
template <typename T>
void printDefs(_Vector< _def<T> >* defs, bool *data){

	if( defs ){

		for( int i=0; i<defs->length; i++ ){

			printf("Def %d----------------------\n", i);
			printDef(&defs->data[i], data);
			printf("\n");
		}
	}
}

//----------------------------- PRINT FILTERS ----------------------------------------------
template <typename T>
void printFilter(_filter<T> *filter, bool *data){

	if( filter ){

		printf("i=%d, ", filter->i);
		printf("w=");
		printArray(&filter->w, data[0]);
	}
}

template <typename T>
void printFilters(_Vector< _filter<T> > *filters, bool *data){

	if( filters ){

		for( int i=0; i<filters->length; i++ ){
		
			printf("Filter %d-----------------------\n", i);
			printFilter(&filters->data[i], data);
		}
	}
}

//----------------------------- PRINT BOX --------------------------------------------------
template <typename T>
void printBox(_box<T> *box, bool data){

	if( box){

		printf("s=%.15f, ", box->s);
		printf("c=%d, ", box->c);
		printf("level=%d, ", box->level);
		printf("xy=");
		printArray(box->xy, data);
	}
}

//----------------------------- PRINT BOXES ------------------------------------------------
template <typename T>
void printBoxes(_Vector< _box<T> > *boxes, bool data){

	if( boxes ){

		for( int i=0; i<boxes->length; i++ ){

			printf("Box[%d]------------------------------\n", i);
			printBox(&boxes->data[i], data);
		}
	}
}

//-------------------------------PRINT MODEL --------------------------------------------
template <typename T>
void printModel(_model<T> *model, bool **data){

	if( model ){

		printf("obj=%f, ", model->obj);
		printf("delta=%f, ", model->delta);
		printf("thresh=%f, ", model->thresh);
		printf("sbin=%d, ", model->sbin);
		printf("interval=%d, ", model->interval);
		printf("len=%d, ", model->len);
		printf("mxasize=");
		printVector(&model->maxsize, data[0][0]);
		printf("\n");
		printDefs(&model->defs, data[1]);
		printFilters(&model->filters, data[2]);
		printComponents(&model->components, data[3]);
	}
}

} // end of namespace

#endif  /*  MYDATAPRINT_H_  */
