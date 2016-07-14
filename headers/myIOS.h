#ifndef MYGETARGS_H_
#define MYGETARGS_H_

#include <stdio.h>

#include "myLibs/myDataTypes.h"
#include "myLibs/myLibs.h"
#include "openCV/myShowImage.h"
#include "xmlLibs/xmlWrite.h"

namespace my{

_Vector< _Vector<char> > *getArguments(char *argumentsText){

	_Vector< _Vector<char> > *args = newVector< _Vector<char> >(true);

	//Count the number of arguments
	for( int i=0; i<(int)strlen(argumentsText); i++ ){

		int c = strcspn(&argumentsText[i]," \n\0");
		if( c > 0 ) {
			args->length++;
			i += c;
		}
	}
	if( args->length == 0 )  return args;

	//Set the arguments to the arguments vector
	createVector(args, args->length);

	args->data[0].data = strtok(argumentsText, " ");
	args->data[0].length = strlen(args->data[0].data);
	for( int i=1; i<args->length; i++ ){

		args->data[i].data = strtok(NULL, " \n");
		args->data[i].length = strlen(args->data[0].data);
	}

	return args;
}

_Vector< _Vector<char> > *getArguments(char *argv[], int argc){

	_Vector< _Vector<char> > *args = createVector< _Vector<char> >(argc-1);

	//Count the number of arguments
	for( int i=1; i<argc; i++ ){

		createVector( &args->data[i-1], strlen(argv[i]) );
		strcpy(args->data[i-1].data, argv[i]);
	}

	return args;
}

template <typename T>
char* setOutput(_Vector< _box<T> > *boxes, _settings<T> *sets, char *image_path){

	if( !boxes ) return NULL;

	if( sets->Output_Type & 1 ) {

		char *xmlPath = (char*) calloc(strlen(image_path)+5, sizeof(char));
		strncpy(xmlPath,image_path, strrchr(image_path,'.')-image_path );
		strcpy(&xmlPath[strlen(xmlPath)],".xml");
		if( !writeXMLBoxes(boxes, xmlPath, sets->XML_DoublePrecision) ) return NULL;
		free(xmlPath);
	}
	if( sets->Output_Type & 6 ) {
		if( !showImage(image_path, boxes, sets->Output_Type) ) return NULL;
	}

	return image_path;
}

} // end of namespace

#endif /* MYGETARGS_H_ */
