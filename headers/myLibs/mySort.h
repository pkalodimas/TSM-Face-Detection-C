
#ifndef MYSORT_H_
#define MYSORT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "myDataTypes.h"
#include "myLibs.h"

namespace my{
	
template <typename T>
T mypivot(T x, T y){ return (T) (x+y)/2; }

//--------------------------- SORT WITH INDEXES ------------------------------------------
template <typename T, typename I>
void sortwi(T *list,int m,int n, I *indexes){

	if( !list || !indexes ) return;

    int i,j,k;
	T key;

    if( m < n)
    {
		k = mypivot(m, n);
        myswap(&list[m], &list[k]);
        myswap(&indexes[m], &indexes[k]);
		key = list[m];
        i = m+1;
        j = n;
        
		while(i <= j){

            while((i <= n) && (list[i] <= key))  i++;

            while((j >= m) && (list[j] > key))  j--;
            
			if( i < j){
				myswap(&list[i], &list[j]);
                myswap(&indexes[i], &indexes[j]);
			}
        }

		myswap(&list[m], &list[j]);
        myswap(&indexes[m], &indexes[j]);
 
		sortwi(list, m, j-1, indexes);
		sortwi(list, j+1, n, indexes);
    }
}

//--------------------------- SORT NO INDEXES ------------------------------------------
template <typename T>
void sort(T *list,int m,int n){

	if( !list ) return;

    int i,j,k;
	T key;

    if( m < n)
    {
		k = mypivot(m, n);
        myswap(&list[m], &list[k]);
       
		key = list[m];
        i = m+1;
        j = n;
        
		while(i <= j){

            while((i <= n) && (list[i] <= key))  i++;

            while((j >= m) && (list[j] > key))  j--;
            
			if( i < j){
                myswap(&list[i], &list[j]);
			}
        }

		myswap(&list[m], &list[j]);
 
		sort(list, m, j-1);
		sort(list, j+1, n);
    }
}

//--------------------------- SORT ------------------------------------------

template <typename T, typename I>
_Vector<T> *sortArray(_Vector<T> *table, _Vector<T> *sortedtable, _Vector<I> *indexes){

	if( !table ) return NULL;

	if( sortedtable ) copyVector(table, sortedtable);	
	else sortedtable = table;

	if( indexes ) {

		createVector(indexes, table->length);
		for( int i=0; i<indexes->length; i++ ) indexes->data[i] = (I) i;
		sortwi(sortedtable->data, 0, sortedtable->length-1, indexes->data);
	}
	else sort(sortedtable->data, 0, sortedtable->length-1);

	return sortedtable;
}

template <typename T, typename I>
_Vector<T> *sortArray(_Vector<T> *table, _Vector<I> *indexes){

	return sortArray(table, (_Vector<T>*) NULL, indexes);
}

template <typename T, typename I>
_Vector<T> *sortArray(_Vector<T> *table){

	return sortArray(table, (_Vector<T>*) NULL, (_Vector<I>*) NULL);
}

} // end of namespace

#endif
