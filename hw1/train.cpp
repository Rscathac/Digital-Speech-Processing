#include<iostream>
#include"hmm.h"
#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define MAX_SAMPLE 10000

using namespace std;

double gamma_prob[MAX_SAMPLE][MAX_SEQ][MAX_STATE];
double epsil_prob[MAX_SAMPLE][MAX_SEQ][MAX_STATE][MAX_STATE];


void forward_algorithm(double forward_prob[MAX_SEQ][MAX_STATE], HMM *hmm, char o[MAX_SEQ]) {
	// Initialization
	memset(forward_prob, 0, sizeof(forward_prob[0][0]) * MAX_SEQ * MAX_STATE);
	int T = strlen(o);
	for(int i = 0; i < hmm->state_num; i++) 
		forward_prob[0][i] = hmm->initial[i] * hmm->observation[(o[0]-'A')][i];
	
	// Induction
	for(int t = 1; t < T; t++) 
		for(int j = 0; j < hmm->state_num; j++) {
			for(int i = 0;  i < hmm->state_num; i++) 
				forward_prob[t][j] += forward_prob[t-1][i] * hmm->transition[i][j];
		
			forward_prob[t][j] *= hmm->observation[(o[t]-'A')][j];
		}
	return;
}

void backward_algorithm(double backward_prob[MAX_SEQ][MAX_STATE], HMM *hmm, char o[MAX_SEQ]) {
	// Initialization
	memset(backward_prob, 0, sizeof(backward_prob[0][0]) * MAX_SEQ * MAX_STATE);
	int T = strlen(o);
	for(int i = 0; i < hmm->state_num; i++)
		backward_prob[T-1][i] = 1;

	// Induction
	for(int t = T-2; t >= 0; t--) 
		for(int i = 0; i < hmm->state_num; i++) 
			for(int j = 0; j < hmm->state_num; j++)
				backward_prob[t][i] += hmm->transition[i][j] * hmm->observation[(o[t+1]-'A')][j] * backward_prob[t+1][j];
	
	return;
}

void gamma_algorithm(double gamma[MAX_SEQ][MAX_STATE], double forward_prob[MAX_SEQ][MAX_STATE], double backward_prob[MAX_SEQ][MAX_STATE], int T, int N) {
	for(int t = 0; t < T; t++) {
		double total_prob = 0;
		for(int i = 0; i < N; i++) 
			total_prob += forward_prob[t][i] * backward_prob[t][i] ;
		for(int i = 0; i < N; i++) {
			gamma[t][i] = forward_prob[t][i] * backward_prob[t][i] / total_prob;
		}
	}

	return;
}

void epsilon_algorithm(double epsil[MAX_SEQ][MAX_STATE][MAX_STATE], double forward_prob[MAX_SEQ][MAX_STATE], double backward_prob[MAX_SEQ][MAX_STATE], HMM *hmm, char o[MAX_SEQ]) {
	int T = strlen(o);
	for(int t = 0; t < T-1; t++) {
		double total_prob = 0;
		for(int i = 0; i < hmm->state_num; i++) {
			for(int j = 0; j < hmm->state_num; j++) {
				total_prob += forward_prob[t][i] * hmm->transition[i][j] * hmm->observation[(o[t+1]-'A')][j] * backward_prob[t+1][j];
			}
		}

		for(int i = 0; i < hmm->state_num; i++) {
			for(int j = 0; j < hmm->state_num; j++) {
				epsil[t][i][j] = forward_prob[t][i] * hmm->transition[i][j] * hmm->observation[(o[t+1]-'A')][j] * backward_prob[t+1][j] / total_prob;
			}
		}
	}

	return;
}


int main(int argc, char *argv[]) {

	HMM hmm_initial;
	loadHMM(&hmm_initial, argv[2]);
//	dumpHMM(stderr, &hmm_initial);
	
	FILE *fp = open_or_die(argv[3], "r");
	
	char each_seq[MAX_SAMPLE][MAX_SEQ];
	int  sample_T[MAX_SAMPLE] = {0};
	int sample_num = 0;

	// read training model
	while(fscanf(fp, "%s", each_seq[sample_num]) > 0) {

		sample_T[sample_num] = strlen(each_seq[sample_num]);
		sample_num++;
	}
	
	fclose(fp);
	// training process

	int iteration = stoi(argv[1]);


	while(iteration--) {
		printf("Remaining iteration : %d\n", iteration);
		for(int n = 0; n < sample_num; n++) {
			double forward_prob[MAX_SEQ][MAX_STATE];
			double backward_prob[MAX_SEQ][MAX_STATE];
			forward_algorithm(forward_prob, &hmm_initial, each_seq[n]);
			backward_algorithm(backward_prob, &hmm_initial, each_seq[n]);
			gamma_algorithm(gamma_prob[n], forward_prob, backward_prob, sample_T[n], hmm_initial.state_num);
			epsilon_algorithm(epsil_prob[n], forward_prob, backward_prob, &hmm_initial, each_seq[n]);
		}

		// revise HMM model
		for(int i = 0; i < hmm_initial.state_num; i++) {
			double gamma_sum = 0;
			for(int j = 0; j < sample_num; j++)
				gamma_sum += gamma_prob[j][0][i];
			hmm_initial.initial[i] = gamma_sum / sample_num;
		}

		for(int i = 0; i < hmm_initial.state_num; i++) {
			for(int j = 0; j < hmm_initial.state_num; j++) {
				double epsil_sum = 0;
				double gamma_sum = 0;
				for(int n = 0; n < sample_num; n++) {
					for(int t = 0; t < sample_T[n]-1; t++) {
						epsil_sum += epsil_prob[n][t][i][j];
						gamma_sum += gamma_prob[n][t][i];
					}
				}
				
				hmm_initial.transition[i][j] = epsil_sum / gamma_sum;
			}
		}

		for(int k = 0; k < hmm_initial.observ_num; k++) {
			for(int j = 0; j < hmm_initial.state_num; j++) {
				double gamma_match = 0;
				double gamma_total = 0;
				for(int n = 0; n < sample_num; n++) {
					for(int t = 0; t < sample_T[n]; t++) {
						if((each_seq[n][t]-'A') == k) 
							gamma_match += gamma_prob[n][t][j];
						gamma_total += gamma_prob[n][t][j];
					}
				}

				hmm_initial.observation[k][j] = gamma_match / gamma_total;
			}	
		}
	}

	dumpHMM(stderr, &hmm_initial);

	FILE *out_fp = open_or_die(argv[4], "w");
	dumpHMM(out_fp, &hmm_initial);

	fclose(out_fp);

	return 0;

}
