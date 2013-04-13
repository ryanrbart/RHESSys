/** @file in_list.c
 *	@brief Determines if a value is in a list of a given length
 * 	returns the position in the list if found otherwise
 *	returns a zero value
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <string.h>

#include "blender.h"
#include "in_list.h"

int in_list(int value, int *list, int length) {

//	int i, fnd;
//
//	fnd = 0;
//
//	for (i = 1; i <= num_pit; i++) {
//		if (inx == list[i])
//			fnd = i;
//	}
//
//	return (fnd);

	for (int i = 1; i <= length; i++) {
		if (value == list[i]) return i;
	}
	return 0;

} /* end in_list */

