
#ifndef MYREADXML_H_
#define MYREADXML_H_

#include <stdio.h>
#include <string.h>

#include "rapidXML/rapidxml.hpp"
#include "rapidXML/rapidxml_utils.hpp"

#include "../myLibs/myDataTypes.h"
#include "../myLibs/mySettings.h"
#include "xmlLibs.h"

using namespace rapidxml;

namespace my{

//---------------------------- READ MODEL --------------------------------------------
template <typename T>
_model<T>* readXMLModel(_model<T> *model, const char *xmlFilePath){

	rapidxml::file<> xmlFile(xmlFilePath);
	xml_document<> doc;
	xml_node<> *xmlModel = NULL;
	if( !model ) model = newModel<T>(true);

/* 	 try
    { */
		doc.parse<0>(xmlFile.data());
/*     }
    catch(rapidxml::parse_error &e)
    {
		printf("\n%s", e.what());
        return NULL;
    } */

	xmlModel = doc.first_node("model");

	model->obj = (T) atof(xmlModel->first_node("obj")->value());
	model->thresh = (T) atof(xmlModel->first_node("thresh")->value());
	model->delta = (T) atof(xmlModel->first_node("delta")->value());

	model->sbin = (int) atoi(xmlModel->first_node("sbin")->value());
	model->interval = (int) atoi(xmlModel->first_node("interval")->value());
	model->len = (int) atoi(xmlModel->first_node("len")->value());

	xml2cVector(xmlModel->first_node("maxsize"), &model->maxsize);
	getDefs(xmlModel->first_node("defs"), &model->defs);
	getFilters(xmlModel->first_node("filters"), &model->filters);
	getComponents(xmlModel->first_node("components"), &model->components);
	
	return model;
}

template <typename T>
_model<T>* readXMLModel(const char *xmlFilePath){

	return readXMLModel((_model<T>*)NULL, xmlFilePath);
}

//---------------------------- READ SETTINGS -----------------------------------------
template <typename T>
_settings<T>* readXMLSettings(_settings<T> *settings, const char *xmlFilePath){

	if( !xmlFilePath ) return NULL;

	rapidxml::file<> xmlFile(xmlFilePath);
	xml_document<> doc;
	xml_node<> *xmlSettings = NULL;
	if( !settings ) settings = newSettings<T>(true);

/* 	try
    { */
		doc.parse<0>(xmlFile.data());
/*     }
    catch(rapidxml::parse_error &e)
    {
		printf("\n%s", e.what());
        return NULL;
    } */

	xmlSettings = doc.first_node("settings");

	settings->Threshold = (T) atof(xmlSettings->first_node("threshold")->value());
	settings->BoxCacheSize = (int) atoi(xmlSettings->first_node("boxCacheSize")->value());
	settings->Nms_Limit = (int) atoi(xmlSettings->first_node("nmsLimit")->value());
	settings->Nms_Overlap = (T) atof(xmlSettings->first_node("nmsOverlap")->value());
	settings->Interval = (int) atoi(xmlSettings->first_node("interval")->value());
	settings->MinLevel = (int) atoi(xmlSettings->first_node("minlevel")->value());
	settings->MaxLevel = (int) atoi(xmlSettings->first_node("maxlevel")->value());
	settings->Output_Type = (int) atoi(xmlSettings->first_node("outputType")->value());
	settings->XML_DoublePrecision = (int) atoi(xmlSettings->first_node("doublePrecision")->value());
	settings->Face_Height = (int) atoi(xmlSettings->first_node("faceHeight")->value());
	settings->Find_Buffer = (int) atoi(xmlSettings->first_node("findBuffer")->value());

	return settings;
}

template <typename T>
_settings<T>* readXMLSettings(const char *xmlFilePath){

	return readXMLSettings((_settings<T> *)NULL, xmlFilePath);
}

} // end of namespace

#endif
