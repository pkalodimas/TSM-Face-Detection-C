
#ifndef MYREADIMAGE_H_
#define MYREADIMAGE_H_

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "../myLibs/myDataTypes.h"
#include "../myLibs/myLibs.h"

using namespace cv;

namespace my{

template <typename T>
_Array<T> *readImage(char *filepath){

	if( !filepath ) return NULL;

	Mat cvimage = imread(filepath, CV_LOAD_IMAGE_COLOR);
	if( !cvimage.data ) return NULL;

	_Array<T> *image = my::createArray<T>(NULL, cvimage.cols, cvimage.rows, 3, true);
	if( image || image->data ) {

		T *glevel = image->data + (image->dims[0] * image->dims[1]);
		T *blevel = glevel + (image->dims[0] * image->dims[1]);

		for( int y=0; y<image->dims[1]; y++ ){
			for( int x=0; x<image->dims[0]; x++ ){

				image->data[x+y*image->dims[0]] = cvimage.data[cvimage.channels()*(cvimage.cols*y + x) + 0];
				glevel[(x+y*image->dims[0])] = cvimage.data[cvimage.channels()*(cvimage.cols*y + x) + 1];
				blevel[(x+y*image->dims[0])] = cvimage.data[cvimage.channels()*(cvimage.cols*y + x) + 2];
			}
		}
	}
	else image = freeArray(image, true);

	cvimage.release();

	return image;
}

} // end of namespace

#endif
