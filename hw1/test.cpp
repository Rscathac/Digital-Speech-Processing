#include"hmm.h"
#include<math.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>


int main(int argc, char *argv[]) {

	HMM hmm[5];
	load_models(argv[1] ,hmm, 5);

	FILE *input_fp = open_or_die(argv[2], "r");
	FILE *output_fp = open_or_die(argv[3], "w");

	char each_seq[MAX_SEQ] = "";
	while(fscanf(input_fp, "%s", each_seq) > 0) {

		int T = strlen(each_seq);

		int best_model = 0;
		double best_pstar = 0;
		for(int model = 0; model < 5; model++) {
			
			double delta[MAX_SEQ][MAX_STATE];
			memset(delta, 0, sizeof(delta[0][0]) * MAX_SEQ * MAX_STATE);
			// initialization
			for(int i = 0; i < hmm[model].state_num; i++)
				delta[0][i] = hmm[model].initial[i] * hmm[model].observation[(each_seq[0]-'A')][i];

			for(int t = 1; t < T; t++) {
				for(int j = 0; j < hmm[model].state_num; j++) {
					double max_path = 0;
					for(int i = 0; i < hmm[model].state_num; i++) {
						double tmp_path = delta[t-1][i] * hmm[model].transition[i][j];
						if(tmp_path > max_path) 
							max_path = tmp_path;
					}
					
					delta[t][j] = max_path * hmm[model].observation[(each_seq[t]-'A')][j];
				}
			}
			
			double pstar = 0;
			for(int i = 0; i < hmm[model].state_num; i++) {
				if(delta[T-1][i] > pstar) {
					pstar = delta[T-1][i];
				}
			}

			if(pstar > best_pstar) {
				best_pstar = pstar;
				best_model = model;
			}
		}


		fprintf(output_fp, "%s %e\n", hmm[best_model].model_name, best_pstar);

	}
	return 0;

}
