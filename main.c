#include "MultiClassTsetlinMachine.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define IMAGE_THRESHOLD 64

int X_train[NUM_EXAMPLES_TRAIN][FEATURES];
int y_train[NUM_EXAMPLES_TRAIN];

int X_test[NUM_EXAMPLES_TEST][FEATURES];
int y_test[NUM_EXAMPLES_TEST];

void read_set(int X[][FEATURES], int y[], int num, const char* images, const char* labels)
{
	FILE * fp;
	int skip;
	unsigned char b;
	
	fp = fopen(images, "r");
	if (fp == NULL) {
		printf("Error opening %s\n", images);
		exit(EXIT_FAILURE);
	}
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	for (int i = 0; i < num; i++) {
		for (int j = 0; j < FEATURES; j++) {
			fread(&b, 1, 1, fp);
			X[i][j] = b > IMAGE_THRESHOLD ? 1 : 0;
		}
	}
	fclose(fp);
	
	fp = fopen(labels, "r");
	if (fp == NULL) {
		printf("Error opening %s\n", labels);
		exit(EXIT_FAILURE);
	}
	fread(&skip, 4, 1, fp);
	fread(&skip, 4, 1, fp);
	for (int i = 0; i < num; i++) {
		fread(&b, 1, 1, fp);
		y[i] = (int) b;
	}
	fclose(fp);
}

void read_data(void)
{
	printf("Reading train data\n");
	read_set(X_train, y_train, NUM_EXAMPLES_TRAIN, "../train-images.idx3-ubyte", "../train-labels.idx1-ubyte");
	printf("Reading test data\n");
	read_set(X_test, y_test, NUM_EXAMPLES_TEST, "../t10k-images.idx3-ubyte", "../t10k-labels.idx1-ubyte");
}


int main(int argc, char**argv)
{
	int epochs = argc>1 ? atoi(argv[1]) : 10; // 30

	srand(time(NULL));

	read_data();

	struct MultiClassTsetlinMachine *mctm = createMultiClassTsetlinMachine();

	printf("Started training (%d epochs)...\n", epochs);
	
	initialize(mctm);
	clock_t start_total = clock();
	fit(mctm, X_train, y_train, NUM_EXAMPLES_TRAIN, epochs);
	clock_t end_total = clock();
	double time_used = ((double) (end_total - start_total)) / CLOCKS_PER_SEC;
	printf("Training finished in: %f s\n", time_used);

	printf("Evaluating test...\n");
	float acc = evaluate(mctm, X_test, y_test, NUM_EXAMPLES_TEST);
	printf("Accuracy: %f\n", acc);
	
	printf("Evaluating train...\n");
	acc = evaluate(mctm, X_train, y_train, NUM_EXAMPLES_TRAIN);
	printf("Accuracy: %f\n", acc);
	
	return 0;
}
