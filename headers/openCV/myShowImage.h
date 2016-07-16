
#ifndef MYSHOWIMAGE_H_
#define MYSHOWIMAGE_H_

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "../myLibs/myDataTypes.h"
#include "../myLibs/myLibs.h"

using namespace cv;

namespace my{

int posemap[13] = {-90, -75, -60, -45, -30, -15, 0, 15, 30, 45, 60, 75, 90};

template <typename T>
bool showImage(char *filepath, _Vector< _box<T> > *boxes, int options){

	if( !filepath || !boxes ) return false;

	Mat cvimage = imread(filepath, CV_LOAD_IMAGE_COLOR);
	if( !cvimage.data ) return false;

	for( int i=0; i<boxes->length; i++ ){

		_box<T> *box = &boxes->data[i];
		
		int box_edge[4];
		box_edge[0] = (int) arraymin(box->xy->data, box->xy->length, 0, box->xy->dims[0]);
		box_edge[1] = (int) arraymin(box->xy->data, box->xy->length, 1, box->xy->dims[0]);
		box_edge[2] = (int) arraymax(box->xy->data, box->xy->length, 2, box->xy->dims[0]);
		box_edge[3] = (int) arraymax(box->xy->data, box->xy->length, 3, box->xy->dims[0]);
		int partsize = (int) box->xy->data[2] - (int) box->xy->data[0];
		
		rectangle(cvimage, cvPoint((int)box_edge[0]-1, (int)box_edge[1]-1), cvPoint((int)box_edge[2]+1, (int)box_edge[3]+1), CV_RGB(0,0,0), 1, 8, 0);

		putText(cvimage, num2string((int)i+1), cvPoint((int)(box_edge[0] + box_edge[2] - partsize)/2, (int)(box_edge[1] - partsize/2)), CV_FONT_HERSHEY_TRIPLEX, 1, CV_RGB(0,0,0), 1, 8, false);

		for( int j=0; j<box->xy->dims[1]; j++ ){

			T *box_line = &box->xy->data[j*box->xy->dims[0]];

			rectangle(cvimage, cvPoint((int)box_line[0], (int)box_line[1]), cvPoint((int)box_line[2], (int)box_line[3]), CV_RGB(0,254,0), 1, 8, 0);
			circle(cvimage, cvPoint((int) (box_line[0]+box_line[2])/2, (int) (box_line[1]+box_line[3])/2), 2, CV_RGB(254,0,0), -1, 8, 0);
		}
	}

	if( options && 2 ) {

		char *image_path = (char*) calloc(strlen(filepath)+10, sizeof(char));
		strncpy(image_path,filepath, strrchr(filepath,'.')-filepath );
		strcpy(&image_path[strlen(image_path)],"-detected");
		strcpy(&image_path[strlen(image_path)],strrchr(filepath,'.'));
		imwrite(image_path, cvimage);
		free(image_path);
	}

	if( options && 4 ) {
	
		namedWindow("Detection Results", WINDOW_AUTOSIZE);
		imshow("Detection Results", cvimage);
		waitKey(0);
	}

	cvimage.release();

	return true;
}

} // end of namespace

#endif
