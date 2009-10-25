	void	input_prompt(int *, int *, char *, char *, char *,  char *, char *,
				char *, char *, char *, char *, char *, char *, char *, char *, char *, char*, char *,
				char *, int, int, int );
  

	void 	ew_horizon(int *, float *, float *, int, int, double, double);
	void	add_roads(struct flow_struct *, int, FILE *, double);
	void	route_roads_to_patches(struct flow_struct *, int, int);
	int		compute_gamma(struct flow_struct *, int, FILE *, double, double, int, int);
	int		compute_upslope_area(struct flow_struct *, int, FILE *, int, double);
	int		compute_dist_from_road(struct flow_struct *, int, FILE *, double);
	int		compute_drainage_density(struct flow_struct *, int, double);
	void	remove_pits(struct flow_struct *, int, int, int, double, FILE *);

	void	input_ascii_int(int *, char *, int, int, int);
	void	input_ascii_float(float *, char *, int, int, int, float);
	void	output_ascii_float(float *, char *, int, int);
	void	input_ascii_sint(short int *, char *, int, int, int);
	void	print_flow_table(int, struct flow_struct *, int, int, double, double, char *, char *, double);
	void	print_stream_table(int, int, struct flow_struct *, int, int, double, double, char *, char *, double, int);
	void	print_drain_stats(int, struct flow_struct *);
	int		build_flow_table(struct flow_struct *, int *, float *, int *, int *, int *,
			int *, int *, int *, float *, float *, float *, FILE *, int, int, int, int, int, int,
			double, double);