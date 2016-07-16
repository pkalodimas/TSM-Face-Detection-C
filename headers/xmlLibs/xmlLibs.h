#ifndef XMLLIBS_H_
#define XMLLIBS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../myLibs/myDataTypes.h"
#include "../myLibs/myLibs.h"
#include "rapidXML/rapidxml.hpp"

using namespace rapidxml;

namespace my{

//---------------------------------------------------------------------------------------------
//--------------------------------------------- DATA ------------------------------------------
//---------------------------------------------------------------------------------------------

//------------------------------DOUBLE 2 STRING ----------------------------------------------

template <typename T>
char *num2string(char *string, T num, int dprecision){

	if( dprecision > 15 ) dprecision = 15;
	else if( dprecision < 0 ) dprecision = 0;

	char format[10];

	switch(myGetType<T>()){
	case _INT: {
		sprintf(format, "%%.d");
		break;
			}
	case _UINT8: {
		sprintf(format, "%%.u");
		break;
			}
	default: {
		sprintf(format, "%%.%df", dprecision);
		break;
			}
	}

	if( string ) sprintf(string, format, num);
	else {
		char temp[30];
		sprintf(temp, format, num);
		string = strcpy( (char*)malloc(strlen(temp)*sizeof(char)), temp );
	}

	return string;
}

template <typename T>
char *num2string(T num, int dprecision){

	return num2string(NULL, num, dprecision);
}

template <typename T>
char *num2string(T num){

	return num2string(NULL, num, 0);
}

template <typename T>
char *array2String(_Array<T>* table, int dprecision){

	if( !table ) return NULL;
	
	char *string = (char*) calloc(table->length, (12+dprecision)*sizeof(char)); // 10=int, 1=',', 1=' ' or '\0'
	char *value = string;

	for( int i=0; i<table->length; i++, value++ ){

		num2string(value, table->data[i], dprecision);
		value = strchr(value, '\0');
		*value = ' ';
	}
	value--;
	value = '\0';
	value = strcpy( (char*)malloc(strlen(string)*sizeof(char)), string );
	free(string);

	return value;
}

char *arraySize2String(int *dims){

	char *string = (char*) calloc(33, sizeof(char));	//3x10=ints, 2x1=' ', 1x1='\0'

	sprintf(string, "%d %d %d", dims[0], dims[1], dims[2]);
	return string;
}

//---------------------------------------------------------------------------------------------
//--------------------------------------------- ARRAYS ----------------------------------------
//---------------------------------------------------------------------------------------------

//------------------------------XML 2 C ARRAY SIZE -------------------------------------------
int *getXmlArraySize(char *size, int *dims){
	
	if( !dims )  dims = (int*) calloc(3, sizeof(int));
	char *dim = strtok(size, " ");

	for( int i=0; i<3 && dim; i++, dim = strtok(NULL, " ") ){ dims[i] = atoi(dim); }

	return dims;
}

int *getXmlArraySize(char *size){

	return getXmlArraySize(size, NULL);
}

//------------------------------XML 2 C ARRAY DATA -------------------------------------------
template <typename T>
T *getXmlArrayData(char *xmldata, int length, T *data){

	if( length == 0 ) return NULL;
	if( !data ) data = (T*) calloc(length, sizeof(T));
	
	char *value = strtok(xmldata, " ");
	for( int i=0; i<length && value; i++, value = strtok(NULL, " ") ){
		data[i] = (T) atof(value);
	}
	return data;
}

template <typename T>
T *getXmlArrayData(char *xmldata, int length){

	return getXmlArrayData(xmldata, length, (T*) NULL);
}

//------------------------------XML 2 C ARRAY -----------------------------------------------
template <typename T>
_Array<T> *xml2cArray(xml_node<> *xmlArray, _Array<T> *carray){

	if( !carray ) carray = (_Array<T>*) malloc(sizeof(_Array<T>));

	getXmlArraySize(xmlArray->first_attribute("size")->value(), carray->dims);
	carray->length = carray->dims[0] * carray->dims[1] * carray->dims[2];
	carray->data = getXmlArrayData<T>(xmlArray->value(), carray->length);

	return carray;
}

template <typename T>
_Array<T> *xml2cArray(xml_node<> *xmlArray){

	return xml2cArray(xmlArray, (_Array<T>*)NULL);
}

//------------------------------XML 2 C VECTOR SIZE ---------------------------------------
int getXmlVectorSize(char *size){
	
	int *dims = getXmlArraySize(size);
	int length = 0;
	
	length = ( dims[0] > dims[1] ) ? dims[0] : dims[1];
	if( dims[2] > length ) length = dims[2];
	return length;
}

//------------------------------XML 2 C VECTOR --------------------------------------------
template <typename T>
_Vector<T> *xml2cVector(xml_node<> *xmlVector, _Vector<T> *cvector){

	if( !cvector ) cvector = (_Vector<T>*) malloc(sizeof(_Vector<T>));

	cvector->length = getXmlVectorSize(xmlVector->first_attribute("size")->value());
	cvector->data = getXmlArrayData<T>(xmlVector->value(), cvector->length);

	return cvector;
}

template <typename T>
_Vector<T> *xml2cVector(xml_node<> *xmlVector){

	return xml2cVector(xmlVector, (_Vector<T>*)NULL);
}

//---------------------------------------------------------------------------------------------
//------------------------------------ DATA STRUCTURES ----------------------------------------
//---------------------------------------------------------------------------------------------

//--------------------------------------- DEFS ------------------------------------------------
template <typename T>
_Vector< _def<T> > *getDefs(xml_node<> *xmlDefs, _Vector< _def<T> > *defs){

	if( !xmlDefs ) return NULL;
	if( !defs ) defs = newVector< _def<T> >(true);

	defs->length = getXmlVectorSize(xmlDefs->first_attribute("size")->value());
	createVector(defs, defs->length, true);

	xml_node<> *xml_w = xmlDefs->first_node("w");
	xml_node<> *xml_i = xmlDefs->first_node("i");
	xml_node<> *xml_anchor = xmlDefs->first_node("anchor");

	for( int i=0; i<defs->length; i++ , xml_i = xml_i->next_sibling("i"), xml_w = xml_w->next_sibling("w"), xml_anchor = xml_anchor->next_sibling("anchor") ){

		defs->data[i].i = (int) atoi(xml_i->value());
		xml2cVector<T>(xml_w, &defs->data[i].w);
		xml2cVector<int>(xml_anchor, &defs->data[i].anchor);
	}

	return defs;
}

template <typename T>
_Vector< _def<T> > *getDefs(xml_node<> *xmlDefs){

	return getDefs(xmlDefs, (_Vector< _def<T> >*) NULL);
}

//---------------------------------------FILTERS ---------------------------------------------
template <typename T>
_Vector< _filter<T> > *getFilters(xml_node<> *xmlFilters, _Vector< _filter<T> > *filters){

	if( !xmlFilters )  return NULL;
	if( !filters ) filters = newVector< _filter<T> >(true);
	
	filters->length = getXmlVectorSize(xmlFilters->first_attribute("size")->value());
	createVector(filters, filters->length, true);

	xml_node<> *xml_i = xmlFilters->first_node("i");
	xml_node<> *xml_w = xmlFilters->first_node("w");

	for( int i=0; i<filters->length; i++ , xml_i = xml_i->next_sibling("i"), xml_w = xml_w->next_sibling("w") ){

		filters->data[i].i = (int) atoi(xml_i->value());
		xml2cArray<T>(xml_w, &(filters->data[i].w));
	}
	
	return filters;
}

template <typename T>
_Vector< _filter<T> > *getFilters(xml_node<> *xmlfilters){

	return getFilters<T>(xmlfilters, NULL);
}

//----------------------------------- COMPONENT ------------------------------------------
template <typename T>
_Vector< _part<T> > *getComponent(xml_node<> *xmlComponent, _Vector< _part<T> > *component){

	if( !xmlComponent ) return NULL;
	if( !component ) component = newVector< _part<T> >(true);

	component->length = getXmlVectorSize(xmlComponent->first_attribute("size")->value());
	createVector((_Vector< _part<T> >*)component, component->length, true);

	xml_node<> *xml_defid = xmlComponent->first_node("defid");
	xml_node<> *xml_filterid = xmlComponent->first_node("filterid");
	xml_node<> *xml_parent = xmlComponent->first_node("parent");

	for( int i=0; i<component->length; i++ , xml_defid = xml_defid->next_sibling("defid"), xml_filterid = xml_filterid->next_sibling("filterid"), xml_parent = xml_parent->next_sibling("parent") ){

		component->data[i].defid = (int) atoi(xml_defid->value());
		component->data[i].filterid = (int) atoi(xml_filterid->value());
		component->data[i].parent = (int) atoi(xml_parent->value());
	}

	return component;
}

template <typename T>
_Vector< _part<T> > *getComponent(xml_node<> *xmlComponent){
	
	return getComponent(xmlComponent, (_Vector< _part<T> >*)NULL);
}

template <typename T>
_Vector< _part<T> > *getComponentFast(xml_node<> *xmlComponent, _Vector< _part<T> > *component){

	if( !xmlComponent ) return NULL;
	if( !component ) component = newVector< _part<T> >(true);

	component->length = getXmlVectorSize(xmlComponent->first_attribute("size")->value());
	createVector((_Vector< _part<T> >*)component, component->length, true);

	xml_node<> *xml_defid = xmlComponent->first_node("defid");
	xml_node<> *xml_filterid = xmlComponent->first_node("filterid");
	xml_node<> *xml_parent = xmlComponent->first_node("parent");
	xml_node<> *xml_sizx = xmlComponent->first_node("sizx");
	xml_node<> *xml_sizy = xmlComponent->first_node("sizy");
	xml_node<> *xml_filterI = xmlComponent->first_node("filterI");
	xml_node<> *xml_defI = xmlComponent->first_node("defI");
	xml_node<> *xml_w = xmlComponent->first_node("w");
	xml_node<> *xml_scale = xmlComponent->first_node("scale");
	xml_node<> *xml_step = xmlComponent->first_node("step");
	xml_node<> *xml_startx = xmlComponent->first_node("startx");
	xml_node<> *xml_starty = xmlComponent->first_node("starty");

	for( int i=0; i<component->length; i++ , 
										xml_defid = xml_defid->next_sibling("defid"), 
										xml_filterid = xml_filterid->next_sibling("filterid"), 
										xml_parent = xml_parent->next_sibling("parent"), 
										xml_sizx = xml_sizx->next_sibling("sizx"), 
										xml_sizy = xml_sizy->next_sibling("sizy"), 
										xml_filterI = xml_filterI->next_sibling("filterI"), 
										xml_defI = xml_defI->next_sibling("defI"), 
										xml_w = xml_w->next_sibling("w"), 
										xml_scale = xml_scale->next_sibling("scale"), 
										xml_step = xml_step->next_sibling("step"), 
										xml_startx = xml_startx->next_sibling("startx"),
										xml_starty = xml_starty->next_sibling("starty") ){

		component->data[i].defid = (int) atoi(xml_defid->value());
		component->data[i].filterid = (int) atoi(xml_filterid->value());
		component->data[i].parent = (int) atoi(xml_parent->value());
		component->data[i].sizx = (int) atoi(xml_sizx->value());
		component->data[i].sizy = (int) atoi(xml_sizy->value());
		component->data[i].filterI = (int) atoi(xml_filterI->value());
		component->data[i].defI = (int) atoi(xml_defI->value());
		component->data[i].scale = (int) atoi(xml_scale->value());
		component->data[i].step = (int) atoi(xml_step->value());
		component->data[i].startx = (int) atoi(xml_startx->value());
		component->data[i].starty = (int) atoi(xml_starty->value());
		xml2cVector<T>(xml_w, &(component->data[i].w));
	}

	return component;
}

template <typename T>
_Vector< _part<T> > *getComponentFast(xml_node<> *xmlComponent){
	
	return getComponentFast(xmlComponent, (_Vector< _part<T> >*)NULL);
}

//----------------------------------- COMPONENTS -----------------------------------------
template <typename T>
_Vector< _Vector< _part<T> > > *getComponents(xml_node<> *xmlComponents, _Vector< _Vector< _part<T> > > *components){

	if( !xmlComponents )  return NULL;
	if( !components )  components = newVector< _Vector< _part<T> > >(true);

	components->length = getXmlVectorSize(xmlComponents->first_attribute("size")->value());
	createVector(components, components->length, true);

	xml_node<> *xml_cell = xmlComponents->first_node("cell");

	for( int i=0; i<components->length; i++, xml_cell = xml_cell->next_sibling("cell") ) {
		
		getComponent(xml_cell, &components->data[i]);  
	}

	return components;
}

template <typename T>
_Vector< _Vector< _part<T> > > *getComponents(xml_node<> *xmlComponents){

	return getComponents(xmlComponents, (_Vector< _Vector< _part<T> > >*) NULL);
}

template <typename T>
_Vector< _Vector< _part<T> > > *getComponentsFast(xml_node<> *xmlComponents, _Vector< _Vector< _part<T> > > *components){

	if( !xmlComponents )  return NULL;
	if( !components )  components = newVector< _Vector< _part<T> > >(true);

	components->length = getXmlVectorSize(xmlComponents->first_attribute("size")->value());
	createVector(components, components->length, true);

	xml_node<> *xml_cell = xmlComponents->first_node("cell");

	for( int i=0; i<components->length; i++, xml_cell = xml_cell->next_sibling("cell") ) {
		
		getComponentFast(xml_cell, &components->data[i]);  
	}

	return components;
}

template <typename T>
_Vector< _Vector< _part<T> > > *getComponentsFast(xml_node<> *xmlComponents){

	return getComponentsFast(xmlComponents, (_Vector< _Vector< _part<T> > >*) NULL);
}

} // end of namespace

#endif
