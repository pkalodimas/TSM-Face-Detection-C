#ifndef MYSETTINGS_H_
#define MYSETTINGS_H_

#define THRESHOLD				-0.65
#define BOXCACHESIZE			10000
#define NMS_LIMIT				10000
#define NMS_OVERLAP				0.3
#define INTERVAL				5
#define MINLEVEL				5
#define MAXSCALE				100
#define FIND_BUFFER				100
#define OUTPUT_TYPE				2
#define DOUBLE_PRECISION 		5
#define FACE_HEIGHT				100

#include "myDataTypes.h"

namespace my{

template <typename T>
struct _settings{
	T Threshold;
	int BoxCacheSize;
	int Nms_Limit;
	float Nms_Overlap;
	int Interval;
	int MinLevel;
	int MaxLevel;
	int Find_Buffer;
	int Output_Type;
	int XML_DoublePrecision;
	int Face_Height;
};

template <typename T>
_settings<T> *newSettings(bool init){

	if( init )  return (_settings<T>*) calloc(1, sizeof(_settings<T>));
	return  (_settings<T>*) malloc(sizeof(_settings<T>));
}

template<typename T>
_settings<T> *defaultSettings(){

	_settings<T> *settings = newSettings<T>(false);

	settings->Threshold = THRESHOLD;
	settings->BoxCacheSize = BOXCACHESIZE;
	settings->Nms_Limit = NMS_LIMIT;
	settings->Nms_Overlap = NMS_OVERLAP;
	settings->Interval = INTERVAL;
	settings->MinLevel = MINLEVEL;
	settings->MaxLevel = MAXSCALE;
	settings->Find_Buffer = FIND_BUFFER;
	settings->Output_Type = OUTPUT_TYPE;
	settings->XML_DoublePrecision = DOUBLE_PRECISION;
	settings->Face_Height = FACE_HEIGHT;

	return settings;
}

template <typename T>
_model<T> *loadSettings2Model(_model<T> *model, _settings<T> *settings){

	if( !model || !settings ) return model;

	model->thresh = settings->Threshold;
	model->interval = settings->Interval;
	model->BoxCacheSize = settings->BoxCacheSize;
	model->Nms_Limit = settings->Nms_Limit;
	model->Nms_Overlap = settings->Nms_Overlap;
	model->MinLevel = settings->MinLevel;
	model->MaxLevel = settings->MaxLevel;
	model->Find_Buffer = settings->Find_Buffer;
	model->Face_Height = settings->Face_Height;

	return model;
}

} // end of namespace


#endif
