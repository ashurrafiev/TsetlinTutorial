#include "MultiClassTsetlinMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#if LOG_ENABLED
	#include "TsetlinLogger.h"
#endif

#include "ParseParams.h"

#define IMAGE_THRESHOLD 64

int X_train[NUM_EXAMPLES_TRAIN][FEATURES];
int y_train[NUM_EXAMPLES_TRAIN];

int X_test[NUM_EXAMPLES_TEST][FEATURES];
int y_test[NUM_EXAMPLES_TEST];

int read_set(int X[][FEATURES], int y[], int num, const char* images, const char* labels)
{
	FILE * fp;
	int skip;
	unsigned char b;
	
	fp = fopen(images, "r");
	if(fp == NULL) {
		printf("Error reading %s\n", images);
		exit(EXIT_FAILURE);
	}
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	for(int i=0; i<num; i++) {
		for(int j=0; j<FEATURES; j++) {
			if(fread(&b, 1, 1, fp)!=1)
				return 0;
			X[i][j] = b > IMAGE_THRESHOLD ? 1 : 0;
		}
	}
	fclose(fp);
	
	fp = fopen(labels, "r");
	if(fp == NULL) {
		printf("Error reading %s\n", labels);
		exit(EXIT_FAILURE);
	}
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	for(int i=0; i<num; i++) {
		if(fread(&b, 1, 1, fp)!=1)
			return 0;
		y[i] = (int) b;
	}
	fclose(fp);
	
	return 1;
}

int read_data(void)
{
	int res = 1;
	res &= read_set(X_train, y_train, NUM_EXAMPLES_TRAIN, INPUT_DATA_PATH "/train-images.idx3-ubyte", INPUT_DATA_PATH "/train-labels.idx1-ubyte");
	res &= read_set(X_test, y_test, NUM_EXAMPLES_TEST, INPUT_DATA_PATH "/t10k-images.idx3-ubyte", INPUT_DATA_PATH "/t10k-labels.idx1-ubyte");
	return res;
}

int main(int argc, char**argv)
{
	int stepSize = 10;
	int steps = 100;

	ParseParamDefs params;
	startParamDefs(&params);
	addIntParam(&params, "-step-size", &stepSize, 0);
	addIntParam(&params, "-steps", &steps, 0);
	addDoubleParam(&params, "-s", &L_RATE, "learning rate s");
	addDoubleParam(&params, "-t", &L_NORM_THRESHOLD, "threshold T (normalised)");
	addIntParam(&params, "-rand-seed", &RAND_SEED, 0);
	addIntParam(&params, "-acc-eval-train", &ACC_EVAL_TRAIN, 0);
	addIntParam(&params, "-acc-eval-test", &ACC_EVAL_TEST, 0);
	#if LOG_ENABLED
		addFlagParam(&params, "-log-tastates", &LOG_TASTATES, 0);
		addFlagParam(&params, "-log-status", &LOG_STATUS, 0);
		addFlagParam(&params, "-log-append", &LOG_APPEND, 0);
		addFlagParam(&params, "-load-state", &LOAD_STATE, 0);
		addStrParam(&params, "-load-state-path", LOAD_STATE_PATH, 1024, 0);
		addFlagParam(&params, "-remap-state", &REMAP_STATE, 0);
		addFlagParam(&params, "-save-state", &SAVE_STATE, 0);
		addStrParam(&params, "-save-state-path", SAVE_STATE_PATH, 1024, 0);
	#endif
	if(!parseParams(&params, argc, argv)) {
		exit(EXIT_FAILURE);
	}

	printf("CLAUSES = %d\n", CLAUSES);
	printf("L_RATE = %f\n", L_RATE);
	printf("L_NORM_THRESHOLD = %f\n", L_NORM_THRESHOLD);
	if(RAND_SEED) {
		printf("Random seed: %u (fixed)\n", RAND_SEED);
		srand(RAND_SEED);
	}
	else {
		time_t seed = time(NULL);
		printf("Random seed: %lu (time)\n", seed);
		srand(seed);
	}
	printf("Reading data...\n");
	
	if(!read_data()) {
		printf("Failed to read data\n");
		return 1;
	}

	struct MultiClassTsetlinMachine *mctm = createMultiClassTsetlinMachine();
	initialize(mctm);
	int step = loadState(mctm);

	LogTAStates logStates;
	startLogTAStates(&logStates);
	LogStatus log;
	startLogStatus(&log);
	
	if(step==0) {
		if(ACC_EVAL_TRAIN>0)
			log.accTrain = evaluate(mctm, X_train, y_train, NUM_EXAMPLES_TRAIN);
		if(ACC_EVAL_TEST>0)
			log.accTest = evaluate(mctm, X_test, y_test, NUM_EXAMPLES_TEST);
		logTAStates(&logStates, step, mctm);
		logStatus(&log, step, mctm);
	}
	
	int epoch = 0;
	int index = 0;
	
	for(int s=0; s<steps; s++) {
		printf("Step: %d (epoch %d) ... ", step, epoch);
		fflush(stdout);
		clock_t start_epoch = clock();

		for(int l=0; l<stepSize; l++) {
			update(mctm, X_train[index], y_train[index]);
			index++;
			if(index>=NUM_EXAMPLES_TRAIN) {
				index = 0;
				epoch++;
			}
		}
		
		clock_t end_epoch = clock();
		double time_used = ((double) (end_epoch - start_epoch)) / CLOCKS_PER_SEC;
		printf("step time: %f\n", time_used);

		step++;
		
		if(ACC_EVAL_TRAIN>0 && step%ACC_EVAL_TRAIN==0) {
			log.accTrain = evaluate(mctm, X_train, y_train, NUM_EXAMPLES_TRAIN);
			printf("\tTrain acc: %f\n", log.accTrain);
		}
		if(ACC_EVAL_TEST>0 && step%ACC_EVAL_TEST==0) {
			log.accTest = evaluate(mctm, X_test, y_test, NUM_EXAMPLES_TEST);
			printf("\tTest acc: %f\n", log.accTest);
		}
		logTAStates(&logStates, step, mctm);
		logStatus(&log, step, mctm);
	}
	if(ACC_EVAL_TRAIN==-1) {
		log.accTrain = evaluate(mctm, X_train, y_train, NUM_EXAMPLES_TRAIN);
		printf("Final train acc: %f\n", log.accTrain);
	}
	if(ACC_EVAL_TEST==-1) {
		log.accTest = evaluate(mctm, X_test, y_test, NUM_EXAMPLES_TEST);
		printf("Final test acc: %f\n", log.accTest);
	}
	
	saveState(mctm, step);

	finishLogTAStates(&logStates);
	finishLogStatus(&log);
	return 0;
}
