#ifndef MYPROFILER_H_
#define MYPROFILER_H_

#include <time.h>
#include <stdio.h>
#include <omp.h>

#include "xmlLibs/rapidXML/rapidxml_print.hpp"
#include "xmlLibs/xmlLibs.h"

using namespace rapidxml;

namespace my{

//-------------------------------------------------------------------------------------
//----------------------------MEMORY PROFILER ----------------------------------------
//-----------------------------------------------------------------------------------

struct _MemProfiler{

	double *time;
	int *memory;
	int *id;
	int length;
	int counter;
};

_MemProfiler * createMemProfiler(int size){

	_MemProfiler *profiler = new _MemProfiler;
	profiler->time = new double[size];
	profiler->memory = new int[size];
	profiler->id = new int[size];
	profiler->length = size;
	profiler->counter = 0;

	return profiler;
}

int memProfilerAdd(_MemProfiler *profiler, int mem, int id, bool checkpoint){

	if( !profiler || profiler->length-1 <= profiler->counter ) return 0;
	
	if( checkpoint ){

		profiler->time[profiler->counter] = omp_get_wtime();
		profiler->memory[profiler->counter] = 0;
		profiler->id[profiler->counter] = id;
		profiler->counter++;
	}

	profiler->time[profiler->counter] = omp_get_wtime();
	profiler->memory[profiler->counter] = mem;
	profiler->id[profiler->counter] = id;
	profiler->counter++;

	return profiler->counter-1;
}

int memProfilerAdd(_MemProfiler *profiler, int mem, int id){

	return memProfilerAdd(profiler, mem, id, false);
}


bool memProfiler2XML(_MemProfiler *profiler, char *filepath, int double_precision){

	if( !profiler || !filepath ) return false;

	xml_document<> xmlDoc;
	xml_node<> *xmlProfiler = xmlDoc.allocate_node(node_element, "MemProfiler", 0, 0, 0);

	for( int i=0; i<profiler->counter; i++ ){

		xml_node<> *xmlEntry = xmlDoc.allocate_node(node_element, "Cell", 0, 0, 0);

		xml_node<> *xmlnodes = xmlDoc.allocate_node(node_element, "time", num2string((double)profiler->time[i], double_precision), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlnodes = xmlDoc.allocate_node(node_element, "memory", num2string((double)profiler->memory[i], 1), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlnodes = xmlDoc.allocate_node(node_element, "id", num2string(profiler->id[i]), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlProfiler->append_node(xmlEntry);
	}

	xmlDoc.append_node(xmlProfiler);
	std::string xmlString;
	print(std::back_inserter(xmlString), xmlDoc, 0);

	FILE *fd = fopen(filepath, "w");
	fwrite(xmlString.c_str(), xmlString.size(), 1, fd);
	fclose(fd);

	return true;
}

void freeMemProfiler(_MemProfiler *profiler){

	delete profiler->time;
	delete profiler->memory;
	delete profiler->id;
	delete profiler;
}

//-------------------------------------------------------------------------------------
//----------------------------TIMELINE PROFILER ----------------------------------------
//----------------------------------------------------------------------------------

struct _TimelineProfiler{

	double *startTime;
	double *stopTime;
	int *id;
	int length;
	int counter;
};

_TimelineProfiler * createTimelineProfiler(int size){

	_TimelineProfiler *profiler = new _TimelineProfiler;
	profiler->startTime = new double[size];
	profiler->stopTime = new double[size];
	profiler->id = new int[size];
	profiler->length = size;
	profiler->counter = 0;

	return profiler;
}

int timelineProfilerStart(_TimelineProfiler *profiler, int id){

	if( !profiler ) return 0;

	profiler->startTime[profiler->counter] = omp_get_wtime();
	profiler->id[profiler->counter] = id;
	profiler->counter++;

	return profiler->counter-1;
}

time_t timelineProfilerStop(_TimelineProfiler *profiler, int index){

	if( !profiler || index >= profiler->length || index < 0 ) return 0;

	profiler->stopTime[index] = omp_get_wtime();
	
	return profiler->stopTime[index];
}

bool timelineProfiler2XML(_TimelineProfiler *profiler, char *filepath, int double_precision){

	if( !profiler || !filepath ) return false;

	xml_document<> xmlDoc;
	xml_node<> *xmlProfiler = xmlDoc.allocate_node(node_element, "TimeLineProfiler", 0, 0, 0);

	for( int i=0; i<profiler->counter; i++ ){

		xml_node<> *xmlEntry = xmlDoc.allocate_node(node_element, "Cell", 0, 0, 0);

		xml_node<> *xmlnodes = xmlDoc.allocate_node(node_element, "start_time", num2string((double)profiler->startTime[i], double_precision), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlnodes = xmlDoc.allocate_node(node_element, "stop_time", num2string((double)profiler->stopTime[i], double_precision), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlnodes = xmlDoc.allocate_node(node_element, "id", num2string(profiler->id[i]), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlProfiler->append_node(xmlEntry);
	}

	xmlDoc.append_node(xmlProfiler);
	std::string xmlString;
	print(std::back_inserter(xmlString), xmlDoc, 0);

	FILE *fd = fopen(filepath, "w");
	fwrite(xmlString.c_str(), xmlString.size(), 1, fd);
	fclose(fd);

	return true;
}

void freeTimelineProfiler(_TimelineProfiler *profiler){

	delete profiler->startTime;
	delete profiler->stopTime;
	delete profiler->id;
	delete profiler;
}


//-------------------------------------------------------------------------------------
//----------------------------TIME PROFILER ----------------------------------------
//----------------------------------------------------------------------------------

struct _TimeProfiler{

	double *startTime;
	double *totalTime;
	int *calls;
	int *id;
	int length;
	int counter;
};

_TimeProfiler * createTimeProfiler(int size){

	_TimeProfiler *profiler = new _TimeProfiler;
	profiler->startTime = new double[size];
	profiler->totalTime = new double[size];
	profiler->calls = new int[size];
	profiler->id = new int[size];
	profiler->length = size;
	profiler->counter = 0;

	return profiler;
}

int timeProfilerCreateTimer(_TimeProfiler *profiler, int id){

	if( !profiler || profiler->length == profiler->counter ) return -1;

	profiler->startTime[profiler->counter] = 0;
	profiler->totalTime[profiler->counter] = 0;
	profiler->calls[profiler->counter] = 0;
	profiler->id[profiler->counter] = id;
	profiler->counter++;

	return profiler->counter-1;
}

time_t timeProfilerStart(_TimeProfiler *profiler, int index){

	if( !profiler || index < 0 || index >= profiler->counter ) return 0;

	profiler->startTime[index] = omp_get_wtime();
	return profiler->startTime[index];
}

time_t timeProfilerStop(_TimeProfiler *profiler, int index){

	if( !profiler || index >= profiler->counter || index < 0 ) return -1;

	if( profiler->startTime[index] > 0 ) {

		profiler->totalTime[index] += omp_get_wtime() - profiler->startTime[index];
		profiler->startTime[index] = 0;
		profiler->calls[index]++;
	}
	return profiler->totalTime[index];
}

bool timeProfiler2XML(_TimeProfiler *profiler, char *filepath, int double_precision){

	if( !profiler || !filepath ) return false;

	xml_document<> xmlDoc;
	xml_node<> *xmlProfiler = xmlDoc.allocate_node(node_element, "TimeProfiler", 0, 0, 0);

	for( int i=0; i<profiler->counter; i++ ){

		xml_node<> *xmlEntry = xmlDoc.allocate_node(node_element, "Cell", 0, 0, 0);

		xml_node<> *xmlnodes = xmlDoc.allocate_node(node_element, "id", num2string(profiler->id[i]), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlnodes = xmlDoc.allocate_node(node_element, "total_time", num2string((double)profiler->totalTime[i], double_precision), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlnodes = xmlDoc.allocate_node(node_element, "calls", num2string(profiler->calls[i]), 0, 0);
		xmlEntry->append_node(xmlnodes);

		xmlProfiler->append_node(xmlEntry);
	}

	xmlDoc.append_node(xmlProfiler);
	std::string xmlString;
	print(std::back_inserter(xmlString), xmlDoc, 0);

	FILE *fd = fopen(filepath, "w");
	fwrite(xmlString.c_str(), xmlString.size(), 1, fd);
	fclose(fd);

	return true;
}

void freeTimeProfiler(_TimeProfiler *profiler){

	delete profiler->startTime;
	delete profiler->totalTime;
	delete profiler->calls;
	delete profiler->id;
	delete profiler;
}

} // end of namespace

#endif
