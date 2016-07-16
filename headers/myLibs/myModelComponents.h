
#ifndef MYMODELCOMPONENTS_H_
#define MYMODELCOMPONENTS_H_

#include <stdio.h>
#include <math.h>
#include <assert.h>

#include "myDataTypes.h"

namespace my{

template <typename T>
_model<T> * modelComponents(_model<T> *model, _featurePyramid<T> *pyra){

	if( !model || !pyra ) return NULL;

	for( int c=0; c<model->components.length; c++ ){

		for( int k=0; k<model->components.data[c].length; k++ ){

			_part<T> *p = &(model->components.data[c].data[k]);
			
			_filter<T> *f = &(model->filters.data[p->filterid-1]);
			p->filterI = f->i;
			p->sizx = f->w.dims[0];
			p->sizy = f->w.dims[1];

			_def<T> *d = &(model->defs.data[p->defid-1]);
			p->defI = d->i;
			copyVector(&(d->w), &(p->w));

			assert(p->parent < k+1);
			if( p->parent > 0 )  p->scale = d->anchor.data[2] + model->components.data[c].data[p->parent-1].scale;
			else  {
				assert( k==0 );
				p->scale = 0;
			}

			p->step = (int) pow((float) 2, d->anchor.data[2]);
			p->startx = d->anchor.data[0] - (p->step-1) * pyra->padx;
			p->starty = d->anchor.data[1] - (p->step-1) * pyra->pady;
		}
	}

	return model;
}

} // end of namespace

#endif
