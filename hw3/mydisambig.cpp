#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Ngram.h"
#include "VocabMap.h"
#include "Vocab.h"

#define SMALL_PROB -100

Vocab voc, ZhuYin, Big5;
LogP delta[maxWordLength][1024] = {{0}};
int backtracking[maxWordLength][1024];
VocabIndex vocabIdx[maxWordLength][1024];
int state_num[maxWordLength] = {0};

void Viterbi(Ngram& lm, VocabMap& map, VocabString* sentence, unsigned int word_counts) {
	
	/* Variables set up */
	memset(&delta, 0.0, sizeof(delta));
	memset(&backtracking, 0, sizeof(backtracking));
	memset(&vocabIdx, 0, sizeof(vocabIdx));
	memset(&state_num, 0, sizeof(state_num));
//	LogP delta[maxWordLength][900] = {{0}};
//	int backtracking[maxWordLength][900];
//	VocabIndex vocabIdx[maxWordLength][900];
//	int state_num[maxWordLength] = {0};

	VocabIndex bigram_context[] = {Vocab_None};
	VocabIndex empty_context[] = {Vocab_None};
	
	/* Initialization */
	VocabMapIter iter(map, ZhuYin.getIndex(sentence[0]));;
	VocabIndex w;
	Prob p;
	for(int i = 0; iter.next(w, p); i++) {
//		printf("%s\n", Big5.getWord(w));
		VocabIndex state = voc.getIndex(Big5.getWord(w));

		/* replace OOV with <unk> */
		if(state == Vocab_None) 
			state = voc.getIndex(Vocab_Unknown);

		/* Unigram of the first word */
		LogP curP = lm.wordProb(state, empty_context);
		
		if(curP == LogP_Zero) 
			curP = SMALL_PROB;

		delta[0][i] = curP;
		backtracking[0][i] = 0;
		vocabIdx[0][i] = w;
		state_num[0]++;

	}


	/* Induction */
	for(int t = 1; t < word_counts; t++) {
		VocabMapIter iter2(map, ZhuYin.getIndex(sentence[t]));
		VocabIndex w;
		Prob p;
		for(int j = 0; iter2.next(w, p); j++) {
//			printf("%s\n", Big5.getWord(w));
			VocabIndex state = voc.getIndex(Big5.getWord(w));
			
			/* replace OOV with <unk> */
			if(state == Vocab_None)
				state = voc.getIndex(Vocab_Unknown);

			LogP logP_max = LogP_Zero;

			/* find the maximum delta(j) from previous time t-1 */
			for(int q = 0; q < state_num[t-1]; q++) {
				VocabIndex pre_state  = voc.getIndex(Big5.getWord(vocabIdx[t-1][q]));
				bigram_context[0] = pre_state;

				/* bigram of current word */
				LogP curP = lm.wordProb(state, bigram_context);
				LogP uniP = lm.wordProb(state, empty_context);

				if(curP == LogP_Zero && uniP == LogP_Zero) 
					curP = SMALL_PROB;
			
				
				curP += delta[t-1][q];
				if(curP > logP_max) {
					logP_max = curP;
					backtracking[t][j] = q;
				}
			}

			delta[t][j] = logP_max;
			vocabIdx[t][j] = w;
			state_num[t]++;			
		}
	}


	/* find the best path */
	LogP logP_max = LogP_Zero;
	int best_idx = 0;

	for(int i = 0; i < state_num[word_counts-1]; i++) {
		if(delta[word_counts-1][i] > logP_max) {
			logP_max = delta[word_counts-1][i];
			best_idx = i;
		}
	}

	/* backtracking */
	VocabString best_seq[maxWordLength];
	best_seq[0] = "<s>";
	best_seq[word_counts-1] = "</s>";

	for(int i = word_counts-1; i > 0; i--) {
		best_seq[i] = Big5.getWord(vocabIdx[i][best_idx]);
		best_idx = backtracking[i][best_idx];
	
	}
	
	
	for(int i = 0; i < word_counts; i++) {
		if(i != word_counts-1)
			printf("%s ", best_seq[i]);
		else
			printf("%s\n", best_seq[i]);
	}
	
	return;
}


int main(int argc, char *argv[]) {

	
	if (argc != 9) {
		puts("Missing arguments");
		return 0;
	}
	
	int ngram_order = atoi(argv[8]);
	Ngram lm(voc, ngram_order);
	VocabMap map(ZhuYin, Big5);

	/* Read language model */
	const char* lm_filename = argv[6];
	File lmFile(lm_filename, "r");
	lm.read(lmFile);
	lmFile.close();

	
	/* Read ZhuYin to Big5 map */
	const char* map_filename = argv[4];
	File mapFile(map_filename, "r");
	map.read(mapFile);
	mapFile.close();

	/* Parse test data */
	const char* text_filename = argv[2];
	File textFile(text_filename, "r");
	char* line = NULL;
	while(line = textFile.getline()) {

		VocabString sentence[maxWordLength];
		unsigned int word_counts = Vocab::parseWords(line, &sentence[1], maxWordLength);
		sentence[0] = "<s>";
		sentence[word_counts+1] = "</s>";
//		printf("%d\n",word_counts);

//		for(int i = 0; i < word_counts+2; i++)
//			printf("%s ", sentence[i]);
//		puts("");

		Viterbi(lm, map, sentence, word_counts+2);
	}


	return 0;
}
