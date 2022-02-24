#include "MultiClassTsetlinMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "TsetlinLogger.h"

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
	int stepSize = argc>1 ? atoi(argv[1]) : 10;
	int steps = argc>2 ? atoi(argv[2]) : 100;

	printf("CLAUSES = %d\n", CLAUSES);
	printf("L_RATE = %f\n", L_RATE);
	#if RAND_SEED
		printf("Random seed: %u (fixed)\n", RAND_SEED);
		srand(RAND_SEED);
	#else
		time_t seed = time(NULL);
		printf("Random seed: %lu (time)\n", seed);
		srand(seed);
	#endif
	printf("Reading data...");
	
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
	
	int epoch = 0;
	int index = 0;
	
	for(int s=0; s<steps; s++) {
		printf("\nStep: %d (epoch %d)\n", step, epoch);
		
		if(ACC_EVAL_TRAIN>0 && (s==0 || (s+1)%ACC_EVAL_TRAIN==0)) {
			log.accTrain = evaluate(mctm, X_train, y_train, NUM_EXAMPLES_TRAIN);
			printf("Train acc: %f\n", log.accTrain);
		}
		if(ACC_EVAL_TEST>0 && (s==0 || (s+1)%ACC_EVAL_TEST==0)) {
			log.accTest = evaluate(mctm, X_test, y_test, NUM_EXAMPLES_TEST);
			printf("Test acc: %f\n", log.accTest);
		}
		logTAStates(&logStates, step, mctm);
		logStatus(&log, step, mctm);

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
		printf("(step time: %f)\n", time_used);
		
		step++;
	}
	if(ACC_EVAL_TRAIN==0) {
		log.accTrain = evaluate(mctm, X_train, y_train, NUM_EXAMPLES_TRAIN);
		printf("Final train acc: %f\n", log.accTrain);
	}
	if(ACC_EVAL_TEST==0) {
		log.accTest = evaluate(mctm, X_test, y_test, NUM_EXAMPLES_TEST);
		printf("Final test acc: %f\n", log.accTest);
	}
	
	saveState(mctm, step);

	finishLogTAStates(&logStates);
	finishLogStatus(&log);
	return 0;
}
