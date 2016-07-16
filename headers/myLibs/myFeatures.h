#ifndef MYFEATURES_H_
#define MYFEATURES_H_

#include <stdio.h>
#include <math.h>

#include "myDataTypes.h"
#include "myLibs.h"

#define eps 0.0001

namespace my{

double uu[9] = { 1.0000, 0.9397, 0.7660, 0.500, 0.1736, -0.1736, -0.5000, -0.7660, -0.9397 };
double vv[9] = { 0.0000, 0.3420, 0.6428, 0.8660, 0.9848, 0.9848, 0.8660, 0.6428, 0.3420 };

//--------------------------------------------- FEATURES -------------------------------------

template <typename T, typename U>
_Array<T> *features(_Array<T> *feats,_Array<U> *image, int sbin, int *pads){

	if( !image || !image->data ) return NULL;
	if( !pads ) pads = (int*) calloc(3,sizeof(int));
	
	int blocks[2];
	blocks[0] = (int) myround( (float)image->dims[0]/(float)sbin );
	blocks[1] = (int) myround( (float)image->dims[1]/(float)sbin );	

	int out[3];
	out[0] = mymax(blocks[0]-2, 0);
	out[1] = mymax(blocks[1]-2, 0);
	out[2] = 32;	//27+4+1;

	int visible[2];
	visible[0] = blocks[0]*sbin;
	visible[1] = blocks[1]*sbin;

	T *hist = (T *) calloc(blocks[0]*blocks[1]*18, sizeof(T));
	T *norm = (T *) calloc(blocks[0]*blocks[1], sizeof(T));
	feats = createArray(feats, out[0] + 2*pads[0], out[1] + 2*pads[1], out[2], true);

	#ifdef OMP_ENABLE
	#pragma omp parallel for
	#endif
	for ( int y=1; y<visible[1]-1; y++ ) {

		for ( int x=1; x<visible[0]-1; x++ ) {
		  
			// first color channel
			U *s = image->data + mymin(x, image->dims[0]-2) + mymin(y, image->dims[1]-2)*image->dims[0];
			T dx = (T) s[1] - s[-1];
			T dy = (T) s[image->dims[0]] - s[-image->dims[0]];
			T v = dx*dx + dy*dy;

			// second color channel
			s += image->dims[0]*image->dims[1];
			T dx2 = (T) s[1] - s[-1];
			T dy2 = (T) s[image->dims[0]] - s[-image->dims[0]];
			T v2 = dx2*dx2 + dy2*dy2;

			if (v2 > v) {
				v = v2;
				dx = dx2;
				dy = dy2;
			} 

			s += image->dims[0]*image->dims[1];
			dx2 = (T) s[1] - s[-1];
			dy2 = (T) s[image->dims[0]] - s[-image->dims[0]];
			v2 = dx2*dx2 + dy2*dy2;

			if (v2 > v) {
				v = v2;
				dx = dx2;
				dy = dy2;
			}

			//snap to one of 18 orientations
			T best_dot = 0;
			int best_o = 0;
			for( int o=0; o<9; o++ ) {
				
				T dot = uu[o]*dx + vv[o]*dy;
				
				if( dot > best_dot ) {
					best_dot = dot;
					best_o = o;
				} else if( -dot > best_dot ) {
					best_dot = -dot;
					best_o = o+9;
				}
			}
      
			// add to 4 histograms around pixel using linear interpolation
			T xp = ((T)x+0.5)/(T)sbin - 0.5;
			T yp = ((T)y+0.5)/(T)sbin - 0.5;
			int ixp = (int)floor(xp);
			int iyp = (int)floor(yp);
			T vx0 = xp-ixp;
			T vy0 = yp-iyp;
			T vx1 = 1.0-vx0;
			T vy1 = 1.0-vy0;
			v = (T) sqrt(v);

			if( ixp >= 0 && iyp >= 0 )  hist[ixp + iyp*blocks[0] + best_o*blocks[0]*blocks[1]] += (T) vx1*vy1*v;
			if( ixp+1 < blocks[0] && iyp >= 0 )  hist[(ixp+1) + iyp*blocks[0] + best_o*blocks[0]*blocks[1]] += (T) vx0*vy1*v;
			if( ixp >= 0 && iyp+1 < blocks[1] )  hist[ixp + (iyp+1)*blocks[0] + best_o*blocks[0]*blocks[1]] += (T) vx1*vy0*v;
			if( ixp+1 < blocks[0] && iyp+1 < blocks[1] )  hist[(ixp+1) + (iyp+1)*blocks[0] + best_o*blocks[0]*blocks[1]] += (T) vx0*vy0*v;
		}
	}

	// compute energy in each block by summing over orientations
	for( int o=0; o<9; o++ ){

		T *src1 = hist + o*blocks[0]*blocks[1];
		T *src2 = hist + (o+9)*blocks[0]*blocks[1];
		for( T *dst=norm; dst<(norm + blocks[1]*blocks[0]); dst++, src1++, src2++ ) { *dst += (T) (*src1 + *src2) * (*src1 + *src2); }

	}

	#ifdef OMP_ENABLE
	#pragma omp parallel for
	#endif
	for( int y=0; y<out[1]; y++ ) {

		for( int x=0; x<out[0]; x++ ) {

			T *dst = feats->data + (x + pads[0]) + (y*feats->dims[0]) + (pads[1]*feats->dims[0]);      
			T *src, *p;
			T n1, n2, n3, n4;

			p = norm + x + y*blocks[0];      
			n4 = (T) (1.0 / sqrt(p[0] + p[1] + p[blocks[0]] + p[blocks[0]+1] + eps));
			p++;
			n2 = (T) (1.0 / sqrt(p[0] + p[1] + p[blocks[0]] + p[blocks[0]+1] + eps));
			p += (blocks[0]-1);
			n3 = (T) (1.0 / sqrt(p[0] + p[1] + p[blocks[0]] + p[blocks[0]+1] + eps));
			p++;
			n1 = (T) (1.0 / sqrt(p[0] + p[1] + p[blocks[0]] + p[blocks[0]+1] + eps));

			T t1 = 0;
			T t2 = 0;
			T t3 = 0;
			T t4 = 0;

			// contrast-sensitive features
			src = hist + (x+1) + (y+1)*blocks[0];
			for( int o=0; o<18; o++ ) {
		
				T h1 = (T) mymin((T)(*src * n1), (T)0.2);
				T h2 = (T) mymin((T)(*src * n2), (T)0.2);
				T h3 = (T) mymin((T)(*src * n3), (T)0.2);
				T h4 = (T) mymin((T)(*src * n4), (T)0.2);
				*dst = (T) (0.5 * (h1 + h2 + h3 + h4));
				t1 += h1;
				t2 += h2;
				t3 += h3;
				t4 += h4;
				dst += feats->dims[0]*feats->dims[1];
				src += blocks[0]*blocks[1];
			}

			// contrast-insensitive features
			src = hist + (x+1) + (y+1)*blocks[0];
			for( int o=0; o<9; o++ ) {

				T sum = src[0] + src[9*blocks[0]*blocks[1]];
				T h1 = (T) mymin((T)sum * n1, (T)0.2);
				T h2 = (T) mymin((T)sum * n2, (T)0.2);
				T h3 = (T) mymin((T)sum * n3, (T)0.2);
				T h4 = (T) mymin((T)sum * n4, (T)0.2);
				*dst = (T) (0.5 * (h1 + h2 + h3 + h4));
				dst += feats->dims[0]*feats->dims[1];
				src += blocks[0]*blocks[1];
			}

			// texture features
			*dst = (T) (0.2357 * t1);
			dst += feats->dims[0]*feats->dims[1];
			*dst = (T) (0.2357 * t2);
			dst += feats->dims[0]*feats->dims[1];
			*dst = (T) (0.2357 * t3);
			dst += feats->dims[0]*feats->dims[1];
			*dst = (T) (0.2357 * t4);

			// truncation feature
			dst += feats->dims[0]*feats->dims[1];
			*dst = (T) 0;
		}
	}

	free(hist);
	free(norm);

	_Array<T> lastTile = *feats;
	lastTile.data = feats->data + feats->dims[0] * feats->dims[1] * (feats->dims[2]-1);
	portraitArray(&lastTile, pads, (T)1);

	return feats;
}

template <typename T>
_Array<T> *features(_Array<T> *image, int sbin, int *pads){

	return features((_Array<T>*)NULL, image, sbin, pads);
}


//-------------------------------------- PORTRAIT ARRAY --------------------------------------
template <typename T>
_Array<T> *portraitArray(_Array<T> *table, int pads[], T padval){

	if( !table || !table->data || !pads ) return table;
	
	T *data = table->data;

	for( int y=0; y<table->dims[1]; y++ ){

		for( int x=0; x<table->dims[0]; x++, data++ ){

			if( y<pads[1] || y>=table->dims[1]-pads[1] || x<pads[0] || x>=table->dims[0]-pads[0] ) *data = padval;
		}
	}

	return table;
}

} // end of namespace

#endif
