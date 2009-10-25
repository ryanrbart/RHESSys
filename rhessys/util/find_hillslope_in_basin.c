/*--------------------------------------------------------------*/
/*                                                              */ 
/*		find_hillslope_in_basin					*/
/*                                                              */
/*  NAME                                                        */
/*		find_hillslope_in_basin					*/
/*                                                              */
/*                                                              */
/*  SYNOPSIS                                                    */
/*  find_hillslope_in_basin( struct basin_object *basin)			*/
/*                                                              */
/*  OPTIONS                                                     */
/*                                                              */
/*  DESCRIPTION                                                 */
/*                                                              */
/*                                                              */
/*                                                              */
/*  PROGRAMMER NOTES                                            */
/*                                                              */
/*                                                              */
/*                                                              */
/*--------------------------------------------------------------*/

#include <stdio.h>
#include "rhessys.h"

struct hillslope_object *find_hillslope_in_basin(
										  int hillslope_ID,
										  struct basin_object *basin)
{
	/*------------------------------------------------------*/
	/*	Local Function Definition. 							*/
	/*------------------------------------------------------*/
	
	/*------------------------------------------------------*/
	/*	Local Variable Definition. 							*/
	/*------------------------------------------------------*/
	int i;
	int fnd;
	struct hillslope_object *hillslope;

	/*--------------------------------------------------------------*/
	/*	find stratum						*/
	/*--------------------------------------------------------------*/
	i = 0;
	fnd = 0;
	while ( (fnd == 0) && (i >= 0) && (i < basin[0].num_hillslopes)) {
		if (basin[0].hillslopes[i][0].ID == hillslope_ID) {
			hillslope = basin[0].hillslopes[i];
			fnd = 1;
		}
		else {
			i += 1;
		}
	}
	if (fnd == 0) {
		fprintf(stderr,
			"FATAL ERROR: Could not find hillslope %d in  basin %d \n",
			hillslope_ID,
			basin[0].ID);
		exit(1);
	}
	return(hillslope);
}/*end find_hillslope_in_basin */