/*--------------------------------------------------------------*/
/* 								*/
/*		update_gw_drainage					*/
/*								*/
/*								*/
/*	NAME							*/
/*	update_gw_drainage -  					*/
/* 		drainage shallow subsurface saturation zone	*/
/*		from each patch to a regional (hillslope scale)	*/
/*		groundwater store				*/
/*		nitrogen is also drained using assumption 	*/
/*		of an exponential decay of N with depth		*/
/*								*/
/*	SYNOPSIS						*/
/*	int update_gw_drainage(					*/
/*			struct patch_object *			*/
/*			struct hillslope_object *		*/
/*			struct command_line_object *		*/
/*			struct date,				*/
/*			)					*/
/*								*/
/*	returns:						*/
/*								*/
/*	OPTIONS							*/
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*	preset code just uses a user assigned loading rate	*/
/*	and all of it is nitrate				*/
/*								*/
/*								*/
/*--------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "rhessys.h"
#include "phys_constants.h"

int update_gw_drainage(
				  struct  patch_object   *patch,
				  struct  hillslope_object *hillslope,
				  struct  command_line_object *command_line,
				  struct	date	current_date)
{
	/*------------------------------------------------------*/
	/*	Local Function Declarations.			*/
	/*------------------------------------------------------*/
	
	double  compute_z_final(
		int,
		double,
		double,
		double,
		double,
		double);


	double compute_N_leached(int,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double,
		double);

	/*------------------------------------------------------*/
	/*	Local Variable Definition. 			*/
	/*------------------------------------------------------*/
	int ok = 1;
	double drainage,sat_store,N_loss;
	double preday_sat_deficit_z, add_field_capacity;

	/*------------------------------------------------------*/
	/*		assume percent of incoming precip	*/
	/*------------------------------------------------------*/
	drainage = patch[0].soil_defaults[0][0].sat_to_gw_coeff * patch[0].rain_throughfall;
	patch[0].rain_throughfall -= drainage;
	patch[0].gw_drainage = drainage;
	hillslope[0].gw.storage += (drainage * patch[0].area / hillslope[0].area);

	/*------------------------------------------------------*/
	/*	determine associated N leached			*/
	/*------------------------------------------------------*/
	if (patch[0].surface_NO3 > ZERO) {
		N_loss = patch[0].soil_defaults[0][0].sat_to_gw_coeff * patch[0].surface_NO3;
		hillslope[0].gw.NO3 += (N_loss * patch[0].area / hillslope[0].area);
		patch[0].ndf.N_to_gw = N_loss;
		patch[0].surface_NO3 -= N_loss;
		}
	
	return (!ok);
} /* end update_gw_drainage.c */