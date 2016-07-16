#ifndef XMLWRITE_H_
#define XMLWRITE_H_

#include <stdio.h>
#include <string>

#include "rapidXML/rapidxml.hpp"
#include "rapidXML/rapidxml_print.hpp"
#include "../myLibs/myDataTypes.h"
#include "xmlLibs.h"

using namespace rapidxml;

namespace my{

template <typename T>
bool writeXMLBoxes(_Vector< _box<T> > *boxes, char *filepath, int dprecision){

	if( !boxes || !filepath ) return false;

	xml_document<> xmlDoc;
	xml_node<> *xmlBoxes = xmlDoc.allocate_node(node_element, "Boxes", 0, 0, 0);

	for( int i=0; i<boxes->length; i++ ){

		xml_node<> *xmlBox = xmlDoc.allocate_node(node_element, "Box", 0, 0, 0);
		//Box.s
		xml_node<> *xmlnodes = xmlDoc.allocate_node(node_element, "s", num2string(boxes->data[i].s, dprecision), 0, 0);
		xmlBox->append_node(xmlnodes);
		//Box.c
		xmlnodes = xmlDoc.allocate_node(node_element, "c", num2string(boxes->data[i].c), 0, 0);
		xmlBox->append_node(xmlnodes);
		//Box.level
		xmlnodes = xmlDoc.allocate_node(node_element, "level", num2string(boxes->data[i].level), 0, 0);
		xmlBox->append_node(xmlnodes);
		//Box.xy
		xmlnodes = xmlDoc.allocate_node(node_element, "xy", array2String((_Array<T>*) boxes->data[i].xy, dprecision), 0, 0);
		xmlBox->append_node(xmlnodes);

		xmlBoxes->append_node(xmlBox);
	}

	xmlDoc.append_node(xmlBoxes);
	std::string xmlString;
	print(std::back_inserter(xmlString), xmlDoc, 0);

	FILE *fd = fopen(filepath, "w");
	fwrite(xmlString.c_str(), xmlString.size(), 1, fd);
	fclose(fd);

	return true;
}

} // end of namespace

#endif
