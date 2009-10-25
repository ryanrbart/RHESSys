/*--------------------------------------------------------------*/
/* 											*/
/*					update_drainage_land			*/
/*											*/
/*	update_drainage_land.c - creates a patch object				*/
/*											*/
/*	NAME										*/
/*	update_drainage_land.c - creates a patch object				*/
/*											*/
/*	SYNOPSIS									*/
/*	void update_drainage_land( 							*/
/*					struct patch_object *patch			*/
/*				 			double,			 	*/
/*				 			double,			 	*/
/*				 			double,			 	*/
/*							int,				*/
/*							int)				*/
/*											*/
/* 											*/
/*											*/
/*	OPTIONS										*/
/*											*/
/*											*/
/*	DESCRIPTION									*/
/*											*/
/*											*/
/*											*/
/*											*/
/*	PROGRAMMER NOTES								*/
/*											*/
/*											*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include "rhessys.h"


void  update_drainage_land(
								 struct patch_object *patch,
								 double time_int,
								 double	sen_m,
								 double	sen_K,
								 double	std_scale,
								 int verbose_flag,
								 int grow_flag)
{
	/*--------------------------------------------------------------*/
	/*	Local function definition.				*/
	/*--------------------------------------------------------------*/
	double  compute_delta_water(
		int,
		double,
		double,
		double,
		double,
		double);
	
	double compute_varbased_returnflow(
		double,
		double,
		double,
		struct litter_object *);

	double compute_varbased_flow(
		double,
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
	
	double compute_transmissivity_curve(
		double,
		double,
		double,
		double);

	double recompute_gamma(	
		struct patch_object *,
		double);


	double compute_infiltration( int,
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
	
	/*--------------------------------------------------------------*/
	/*	Local variable definition.				*/
	/*--------------------------------------------------------------*/
	int j, d;
	double m, Ksat;
	double N_leached_to_patch, N_leached_to_stream;
	double NO3_leached_to_surface; /* kg/m2 */
	double NH4_leached_to_surface; /* kg/m2 */
	double N_leached_total; /* kg/m2 */
	double route_to_surface;  /* m3 */
	double return_flow,route_to_patch ;  /* m3 */
	double available_sat_water; /* m3 */
	double Qin, Qout;  /* m */
	double innundation_depth, infiltration; /* m */
	double total_gamma;
	double Nin, Nout; /* kg/m2 */ 
	double t1,t2,t3;

	struct patch_object *neigh;
	route_to_patch = 0.0;
	route_to_surface = 0.0;

	N_leached_to_patch = 0.0;
	N_leached_to_stream = 0.0;
	NO3_leached_to_surface = 0.0;
	NH4_leached_to_surface = 0.0;

	/*--------------------------------------------------------------*/
	/*	m and K are multiplied by sensitivity analysis variables */
	/*--------------------------------------------------------------*/

	m = patch[0].soil_defaults[0][0].m ;
	Ksat = patch[0].soil_defaults[0][0].Ksat_0 ;
	d=0;
	/*--------------------------------------------------------------*/
	/*	recalculate gamma based on current saturation deficits  */
	/*      to account the effect of changes in water table slope 	*/
	/*--------------------------------------------------------------*/
	total_gamma = recompute_gamma(patch, patch[0].innundation_list[d].gamma);

	available_sat_water = max(((patch[0].soil_defaults[0][0].soil_water_cap
			- max(patch[0].sat_deficit,0.0))
			* patch[0].area),0.0);

	/*------------------------------------------------------------*/
	/*	calculate amuount of water output to patches			*/
	/*	this only computes subsurface flow, not overland flow	*/
	/*-----------------------------------------------------------*/


	route_to_patch = sen_K * time_int * compute_varbased_flow(patch[0].std * std_scale, total_gamma, m,
		patch[0].soil_defaults[0][0].active_zone_z,
		patch[0].sat_deficit,
		patch[0].soil_defaults[0][0].soil_water_cap) ;

	if (route_to_patch < 0.0) route_to_patch = 0.0;
	if ( route_to_patch > available_sat_water) 
		route_to_patch *= (available_sat_water)/(route_to_patch);
	/*--------------------------------------------------------------*/
	/* compute Nitrogen leaching amount				*/
	/*--------------------------------------------------------------*/
	if (grow_flag > 0) {
		Nout = compute_N_leached(
			verbose_flag,
			patch[0].soil_ns.nitrate,
			route_to_patch / patch[0].area,
			patch[0].sat_deficit,
			patch[0].soil_defaults[0][0].soil_water_cap,
			m,
			sen_K * total_gamma / patch[0].area * time_int,
			patch[0].soil_defaults[0][0].porosity_0,
			patch[0].soil_defaults[0][0].porosity_decay,
			patch[0].soil_defaults[0][0].N_decay_rate,
			patch[0].soil_defaults[0][0].active_zone_z,
			patch[0].soil_defaults[0][0].soil_depth,
			patch[0].soil_defaults[0][0].mobile_N_proportion);
		N_leached_to_patch = Nout * patch[0].area;
		patch[0].soil_ns.Qout += Nout;

	}

	
	patch[0].Qout += (route_to_patch / patch[0].area);


	/*--------------------------------------------------------------*/
	/*	calculate any return flow associated with this patch	*/
	/*	and route any infiltration excess			*/
	/*	return flow is flow leaving patch (i.e surface_Qout)  	*/
	/*	note that return flow that becomes detention storage   */
	/*	is added to surface_Qin					*/
	/*	similarly with associated nitrogen			*/
	/* 	note we move unsat_storage into saturated storage in this case */
	/*	saturated zone will be updated in compute_subsurface_routing	*/
	/*	i.e becomes part of Qout				*/
	/*--------------------------------------------------------------*/
	if ((patch[0].sat_deficit-patch[0].rz_storage-patch[0].unsat_storage) < -1.0*ZERO) {
		return_flow = compute_varbased_returnflow(patch[0].std * std_scale, 
			patch[0].rz_storage+patch[0].unsat_storage,
			patch[0].sat_deficit, &(patch[0].litter));
		patch[0].detention_store += return_flow;  
		patch[0].sat_deficit = 0.0;
		patch[0].unsat_storage = 0.0;
		patch[0].rz_storage = 0.0;
	}
	/*--------------------------------------------------------------*/
	/*	calculated any N-transport associated with return flow  */
	/*	-note available N reduced by what has already been 	*/
	/*	we assume that only nitrate follows return flow		*/
	/*	lost in subsurface flow routing				*/
	/*--------------------------------------------------------------*/
		if (grow_flag > 0) {
			Nout = compute_N_leached(
				verbose_flag,
				patch[0].soil_ns.nitrate - (N_leached_to_patch/patch[0].area),
				return_flow,
				0.0,
				0.0,
				m,
				sen_K * total_gamma / patch[0].area * time_int,
				patch[0].soil_defaults[0][0].porosity_0,
				patch[0].soil_defaults[0][0].porosity_decay,
				patch[0].soil_defaults[0][0].N_decay_rate,
				patch[0].soil_defaults[0][0].active_zone_z,
				patch[0].soil_defaults[0][0].soil_depth,
				patch[0].soil_defaults[0][0].mobile_N_proportion);
			patch[0].surface_NO3 += Nout;
			patch[0].soil_ns.Qout += Nout;
		}
	
	/*--------------------------------------------------------------*/
	/*	route water and nitrogen lossed due to infiltration excess */
	/*--------------------------------------------------------------*/
	if ( (patch[0].detention_store > patch[0].soil_defaults[0][0].detention_store_size) &&
		(patch[0].detention_store > ZERO) ){

		Qout = (patch[0].detention_store - patch[0].soil_defaults[0][0].detention_store_size);
		if (grow_flag > 0) {
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NO3;
			NO3_leached_to_surface = Nout * patch[0].area;
			patch[0].surface_NO3 -= Nout;
			Nout = (min(1.0, (Qout/ patch[0].detention_store))) * patch[0].surface_NH4;
			NH4_leached_to_surface = Nout * patch[0].area;
			patch[0].surface_NH4 -= Nout;
			}
		route_to_surface = (Qout *  patch[0].area);
		patch[0].detention_store -= Qout;
		patch[0].surface_Qout += Qout;

		}
			

	if (NO3_leached_to_surface < 0.0)
		printf("WARNING %d %lf",patch[0].ID, NO3_leached_to_surface);

	/*--------------------------------------------------------------*/
	/*	route flow to neighbours				*/
	/*	route n_leaching if grow flag specfied			*/
	/*--------------------------------------------------------------*/

	/*--------------------------------------------------------------*/
	/* regular downslope routing */
	/*--------------------------------------------------------------*/
	d=0;
	for (j = 0; j < patch[0].innundation_list[d].num_neighbours; j++) {

		neigh = patch[0].innundation_list[d].neighbours[j].patch;  
		/*--------------------------------------------------------------*/
		/* first transfer subsurface water and nitrogen */
		/*--------------------------------------------------------------*/
		Qin =	(patch[0].innundation_list[d].neighbours[j].gamma * route_to_patch) / neigh[0].area;
		if (Qin < 0) printf("\n warning negative routing from patch %d with gamma %lf", patch[0].ID, total_gamma);
		if (grow_flag > 0) {
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * N_leached_to_patch) 
				/ neigh[0].area;
			neigh[0].soil_ns.Qin += Nin;
			neigh[0].ndf.sminn_leached -= Nin;
			}
		neigh[0].Qin += Qin;
	}

	/*--------------------------------------------------------------*/
	/* surface downslope routing */
	/*--------------------------------------------------------------*/
	/*--------------------------------------------------------------*/
	/* determine which innundation depth to consider		*/
	/*--------------------------------------------------------------*/
	if (patch[0].num_innundation_depths > 0) {
		innundation_depth = patch[0].detention_store + route_to_surface; 
		d=0;
		while ((innundation_depth > patch[0].innundation_list[d].critical_depth) 
			&& (d < patch[0].num_innundation_depths-1)) {
			d++;}
		}
	else d=0;
	for (j = 0; j < patch[0].innundation_list[d].num_neighbours; j++) {

		neigh = patch[0].innundation_list[d].neighbours[j].patch;  

		/*--------------------------------------------------------------*/
		/* now transfer surface water and nitrogen */
		/*	- first nitrogen					*/
		/*--------------------------------------------------------------*/
		if (grow_flag > 0) {
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * NO3_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NO3 += Nin;
			Nin = (patch[0].innundation_list[d].neighbours[j].gamma * NH4_leached_to_surface) / neigh[0].area;
			neigh[0].surface_NH4 += Nin;
			}
		
		/*--------------------------------------------------------------*/
		/*	- now surface water 					*/
		/*	surface stores should be updated to facilitate transfer */
		/* added net surface water transfer to detention store		*/
		/*--------------------------------------------------------------*/

		Qin = (patch[0].innundation_list[d].neighbours[j].gamma * route_to_surface) / neigh[0].area;
		neigh[0].detention_store += Qin;
		neigh[0].surface_Qin += Qin;
		
		/*--------------------------------------------------------------*/
		/* try to infiltrate this water					*/ 
		/* use time_int as duration */
		/*--------------------------------------------------------------*/
		if (neigh[0].detention_store > ZERO) {
			if (neigh[0].rootzone.depth > ZERO) {
			infiltration = compute_infiltration(
				verbose_flag,
				neigh[0].sat_deficit_z,
				neigh[0].rootzone.S,
				neigh[0].Ksat_vertical,
				neigh[0].soil_defaults[0][0].Ksat_0_v,
				neigh[0].soil_defaults[0][0].mz_v,
				neigh[0].soil_defaults[0][0].porosity_0,
				neigh[0].soil_defaults[0][0].porosity_decay,
				(neigh[0].detention_store),	
				time_int,
				neigh[0].soil_defaults[0][0].theta_psi_curve);
			}
			else {
			infiltration = compute_infiltration(
				verbose_flag,
				neigh[0].sat_deficit_z,
				neigh[0].S,
				neigh[0].Ksat_vertical,
				neigh[0].soil_defaults[0][0].Ksat_0_v,
				neigh[0].soil_defaults[0][0].mz_v,
				neigh[0].soil_defaults[0][0].porosity_0,
				neigh[0].soil_defaults[0][0].porosity_decay,
				(neigh[0].detention_store),	
				time_int,
				neigh[0].soil_defaults[0][0].theta_psi_curve);
			}
		}
		else infiltration = 0.0;
		/*--------------------------------------------------------------*/
		/* added an surface N flux to surface N pool	and		*/
		/* allow infiltration of surface N				*/
		/*--------------------------------------------------------------*/
		if ((grow_flag > 0 ) && (infiltration > ZERO)) {
			neigh[0].soil_ns.Qin += ((infiltration / neigh[0].detention_store) * neigh[0].surface_NO3);
			neigh[0].surface_NO3 -= ((infiltration / neigh[0].detention_store) * neigh[0].surface_NO3);
			neigh[0].soil_ns.sminn += ((infiltration / neigh[0].detention_store) * neigh[0].surface_NH4);
			neigh[0].surface_NH4 -= ((infiltration / neigh[0].detention_store) * neigh[0].surface_NH4);
		}

		if (infiltration > neigh[0].sat_deficit - neigh[0].unsat_storage - neigh[0].rz_storage) {
			neigh[0].sat_deficit -= (infiltration + neigh[0].unsat_storage + neigh[0].rz_storage);
			neigh[0].unsat_storage = 0.0; 
			neigh[0].rz_storage = 0.0; 
			neigh[0].field_capacity = 0.0; 
			neigh[0].rootzone.field_capacity = 0.0; 
		}

		else if ((neigh[0].sat_deficit > neigh[0].rootzone.potential_sat) &&
			(infiltration > neigh[0].rootzone.potential_sat - neigh[0].rz_storage)) {
		/*------------------------------------------------------------------------------*/
		/*		Just add the infiltration to the rz_storage and unsat_storage	*/
		/*------------------------------------------------------------------------------*/
			neigh[0].unsat_storage += infiltration - (neigh[0].rootzone.potential_sat - neigh[0].rz_storage);
			neigh[0].rz_storage = neigh[0].rootzone.potential_sat;
		}								
		/* Only rootzone layer saturated - perched water table case */
		else if ((neigh[0].sat_deficit > neigh[0].rootzone.potential_sat) &&
			(infiltration <= neigh[0].rootzone.potential_sat - neigh[0].rz_storage)) {
			/*--------------------------------------------------------------*/
			/*		Just add the infiltration to the rz_storage	*/
			/*--------------------------------------------------------------*/
			neigh[0].rz_storage += infiltration;
		}
		else if ((neigh[0].sat_deficit <= neigh[0].rootzone.potential_sat) &&
			(infiltration <= neigh[0].sat_deficit - neigh[0].rz_storage - neigh[0].unsat_storage)) {
			neigh[0].rz_storage += neigh[0].unsat_storage;		
			/* transfer left water in unsat storage to rootzone layer */
			neigh[0].unsat_storage = 0;
			neigh[0].rz_storage += infiltration;
			neigh[0].field_capacity = 0;
		}

		neigh[0].detention_store -= infiltration;

	}

	N_leached_total = (N_leached_to_patch + NO3_leached_to_surface + NH4_leached_to_surface);
	patch[0].soil_ns.nleached_snk += N_leached_total / patch[0].area;
	patch[0].ndf.sminn_leached +=  N_leached_total / patch[0].area; 


} /*end update_drainage_land.c*/
