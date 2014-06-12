/*--------------------------------------------------------------*/
/* 								*/
/*		compute_stage_storage			*/
/*								*/
/*	NAME							*/
/*	compute_stage_storage - estimates water volume	*/
/*		of SCM given a water depth	*/
/*		Returned value is a table of stages and storages and surface areas	*/
/*      Assumes scm is a rectauglar frustrum */
/*								*/
/*								*/
/*	SYNOPSIS						*/
/*	compute_stage_storage(				*/
/*				double,         */
/*              double,         */
/*              double,         */
/*              double)			*/
/*								*/
/*	returns:						*/
/*	stagestorage - 3x1000 matrix to be put pointed to as patch variable */
/*  "scm_stage_storage"								*/
/*  Columns are (1) stage, (2) volume storage and (3) surface area */
/*								*/
/*	DESCRIPTION						*/
/*								*/
/*	computes volume stored in scm over a range of depths */
/*  Depths run between 0 and maxH - specified in default file */
/*								*/
/*								*/
/*	PROGRAMMER NOTES					*/
/*--------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>
#include "rhessys.h"



double	*compute_stage_storage(
                    double scm_max_area,
                    double scm_max_H,
                    double scm_LtoW,
                    double scm_SS)
{

 
    /*--------------------------------------------------------------*/
    /*	Local Function Definintion                                */
    /*--------------------------------------------------------------*/   
     
    void    *alloc( size_t, char *, char *);
    
    /*--------------------------------------------------------------*/
    /*	Define  local variables                                 */
    /*--------------------------------------------------------------*/
    int l,k;
    double scm_min_area, scm_max_length, scm_max_width, scm_max_volume; /* m^2, m^2, m, m, m^3 */
    double htmp, vtmp, atmp;
    //double *stagestorage = (double *) alloc((3*1000*sizeof(double)),"stages","compute_stage_storage");
    //double (*stagestorage)[1000] = (double *) calloc(3*1000,sizeof(double));
    double **stagestorage; //method from: http://www.cs.swarthmore.edu/~newhall/unixhelp/C_arrays.html - still have warning when returning "incompatible pointer"
    
    stagestorage = (double **)malloc(sizeof(double *)*1000);
    for (l=0; l<1000; l++){
     stagestorage[l] = (double *)malloc(sizeof(double)*3);
    }
    
    /*--------------------------------------------------------------*/
    /*  Compute Relevant scm Geometric Features               */
    /*--------------------------------------------------------------*/
    scm_max_width = pow((scm_max_area / scm_LtoW), 0.5);
    scm_max_length = pow((scm_max_area * scm_LtoW), 0.5);
    scm_min_area = (scm_max_length - 2*(scm_SS * scm_max_H))*(scm_max_width - 2*(scm_SS*scm_max_H));
    scm_max_volume = scm_max_H/3 * (scm_max_area + scm_min_area - pow((scm_max_area-scm_min_area),0.5));
    
    /*--------------------------------------------------------------*/
    /*  Compute Lookup Table for Stage-Storage Relationship     */
    /*--------------------------------------------------------------*/
    

    for (k = 0; k < 1000; k++) {
        htmp               = (k/1000) * scm_max_H;
        atmp               = (scm_max_length - 2 * scm_SS * ( scm_max_H - htmp ))*(scm_max_width - 2 * scm_SS * (scm_max_width - htmp));
        vtmp               = htmp/3 * (atmp + scm_min_area - pow((atmp - scm_min_area),0.5));
        stagestorage[k][0] = htmp;
        stagestorage[k][1] = vtmp;
        stagestorage[k][2] = atmp;
    }
    return(stagestorage); 
    
    
    
    
} /*compute_stage_storage*/
