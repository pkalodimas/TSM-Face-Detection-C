#ifndef MYDATATYPES_H_
#define MYDATATYPES_H_

typedef unsigned char uint8;

namespace my{

template <typename T> 
struct _Vector{

	T *data;
	int length;
};

template <typename T> 
struct _Array{

	T *data;
	int dims[3];
	int length;
};

template <typename T>
struct _def{

	int i;
	_Vector<T> w;
	_Vector<int> anchor;
};

template <typename T>
struct _filter{

	int i;
	_Array<T> w;
};

template <typename T>
struct _featurePyramid{

	_Vector< _Array<T>* > feat;
	_Vector<float> scale;
	int interval;
	int imx;
	int imy;
	int padx;
	int pady;
};

template <typename T>
struct _part{

	int defid;
	int filterid;
	int parent;
	int sizx;
	int sizy;
	int filterI;
	int defI;
	_Vector<T> w;
	int scale;
	int startx;
	int starty;
	int step;
	int level;
	_Array<T> *score;
	_Array<int> *Ix;
	_Array<int> *Iy;
};

template <typename T>
struct _box{

	T s;
	int c;
	int level;
	_Array<T> *xy;
};

template <typename T>
struct _model{

	//Settings
	int BoxCacheSize;
	int Nms_Limit;
	float Nms_Overlap;
	int MinLevel;
	int MaxLevel;
	int Find_Buffer;
	int Face_Height;
	
	//Model
	float obj;
	float delta;
	T thresh;
	int sbin;
	int interval;
	int len;
	_Vector<int> maxsize;
	_Vector< _Vector< _part<T> > > components;
	_Vector< _def<T> > defs;
	_Vector< _filter<T> > filters;
};

typedef enum {

	_NONE,
	_DOUBLE,
	_INT,
	_INT64,
	_FLOAT,
	_UINT8,
	_VECTOR,
	_ARRAY
} _dataTypes;

} // end of namespace
 
#endif
