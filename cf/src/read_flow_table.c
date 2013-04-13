/** @file read_flow_table.c
 *
 *	Not currently used
 */
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>

#include "main.h"
#include "blender.h"
#include "fileio.h" 
#include "glb.h"
#include "check_neighbours.h"
#include "zero_flow_table.h"
#include "find_patch.h"

int read_flow_table(flow_table, maxr, maxc, cell)
	struct flow_struct *flow_table;int maxr, maxc;double cell;

{

	/* local variable declarations */
	int num_patches;
	int inx;
	int r, c, pch;
	int* patch = 0; //CHRIS
	int* zone = 0; //CHRIS
	int* hill = 0;  //chu
	int* dem = 0;  //chu
	int *stream = 0;   //chu
	int sc_flag = STREAM_CONNECTIVITY_NONE; //chu
	FILE *fl = 0;  //chu

	/* local function definitions */
	//void zero_flow_table();

	num_patches = 0;

	// Invalid call
	//zero_flow_table(flow_table);

	for (r = 0; r < maxr; r++) {
		for (c = 0; c < maxc; c++) {
			inx = r * maxc + c;
			if (patch[inx] == NO_DATA) {
				printf(
						"error in patch file use of NO_DATA as a patch label not allowed\n");
				exit(EXIT_FAILURE);
			}
			pch = find_patch(num_patches, flow_table, patch[inx], zone[inx],
					hill[inx]);
			if (pch == 0) {
				num_patches += 1;
				pch = num_patches;
			}
			flow_table[pch].patchID = patch[inx];
			flow_table[pch].hillID = hill[inx];
			flow_table[pch].zoneID = zone[inx];
			flow_table[pch].area += 1;
			flow_table[pch].x += (float) (1.0 * r);
			flow_table[pch].y += (float) (1.0 * c);
			flow_table[pch].z += (float) (1.0 * dem[inx]);

			flow_table[pch].num_adjacent += check_neighbours(r, c, patch, zone,
					hill, stream, &flow_table[pch],
					flow_table[pch].num_adjacent, fl, maxr, maxc, sc_flag,
					cell);  //chu

		}

	}

	printf("\n Total number of patches is %d", num_patches);

	return (num_patches);

}

