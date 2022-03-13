#ifndef _MULTI_CLASS_TSETLIN_MACHINE_H_
#define _MULTI_CLASS_TSETLIN_MACHINE_H_

#include <stdio.h>

#include "TsetlinMachine.h"

struct MultiClassTsetlinMachine {
	TsetlinMachine tsetlinMachines[CLASSES];
};

void initialize(MultiClassTsetlinMachine* mctm) {
	for(int i=0; i<CLASSES; i++) {
		initialize(&mctm->tsetlinMachines[i]);
	}
}

MultiClassTsetlinMachine* createMultiClassTsetlinMachine() {
	struct MultiClassTsetlinMachine *mctm = (MultiClassTsetlinMachine *)malloc(sizeof(struct MultiClassTsetlinMachine));
	initialize(mctm);
	return mctm;
}

void remapState(MultiClassTsetlinMachine* mctm) {
	for(int i=0; i<CLASSES; i++)
		for(int j=0; j<CLAUSES; j++)
			for(int k=0; k<LITERALS; k++) {
				int* ta = &mctm->tsetlinMachines[i].clauses[j].ta[k];
				if(INCLUDE_LITERAL(*ta))
					*ta = 1;
				else
					*ta = -NUM_STATES+1;
			}
}

int loadState(MultiClassTsetlinMachine* mctm) {
	int step = 0;
	if(LOAD_STATE) {
		FILE* fp = fopen(LOAD_STATE_PATH, "rt");
		if(fp == NULL) {
			printf("Error reading %s\n", LOAD_STATE_PATH);
			exit(EXIT_FAILURE);
		}
		fscanf(fp, "%d", &step);
		for(int i=0; i<CLASSES; i++)
			for(int j=0; j<CLAUSES; j++)
				for(int k=0; k<LITERALS; k++)
					fscanf(fp, "%d", &mctm->tsetlinMachines[i].clauses[j].ta[k]);
		fclose(fp);
	}

	if(REMAP_STATE)
		remapState(mctm);
	return step;
}

void saveState(MultiClassTsetlinMachine* mctm, int step) {
	if(SAVE_STATE) {
		FILE* fp = fopen(SAVE_STATE_PATH, "wt");
		if(fp == NULL) {
			printf("Error writing %s\n", SAVE_STATE_PATH);
			exit(EXIT_FAILURE);
		}
		fprintf(fp, "%d", step);
		for(int i=0; i<CLASSES; i++)
			for(int j=0; j<CLAUSES; j++)
				for(int k=0; k<LITERALS; k++)
					fprintf(fp, "\t%d", mctm->tsetlinMachines[i].clauses[j].ta[k]);
		fprintf(fp, "\n");
		fclose(fp);
	}
}

int inferClass(MultiClassTsetlinMachine* mctm, int input[FEATURES]) {
	int maxClassSum = score(&mctm->tsetlinMachines[0], input);
	int maxClass = 0;
	for(int i=1; i<CLASSES; i++) {	
		int classSum = score(&mctm->tsetlinMachines[i], input);
		if(maxClassSum < classSum) {
			maxClassSum = classSum;
			maxClass = i;
		}
	}
	return maxClass;
}

double evaluate(MultiClassTsetlinMachine* mctm, int inputs[][FEATURES], int labels[], int numberOfExamples) {
	int errors = 0;
	for(int l=0; l<numberOfExamples; l++) {
		if(inferClass(mctm, inputs[l])!=labels[l])
			errors++;
	}
	return 1.0 - errors / (double)numberOfExamples;
}

double evaluateClass(MultiClassTsetlinMachine* mctm, int inputs[][FEATURES], int labels[], int numberOfExamples, int cls) {
	int errors = 0;
	int count = 0;
	for(int l=0; l<numberOfExamples; l++) {
		if(cls==labels[l]) {
			count++;
			if(inferClass(mctm, inputs[l])!=cls)
				errors++;
		}
	}
	if(count==0)
		return 0.0;
	else
		return 1.0 - errors / (double)count;
}


void update(MultiClassTsetlinMachine* mctm, int input[], int output) {
	update(&mctm->tsetlinMachines[output], input, 1);

	int negativeTargetClass = rand() % CLASSES;
	while(negativeTargetClass == output) {
		negativeTargetClass = rand() % CLASSES;
	}

	update(&mctm->tsetlinMachines[negativeTargetClass], input, 0);
}

void fit(MultiClassTsetlinMachine* mctm, int inputs[][FEATURES], int outputs[], int numberOfExamples) {
	for(int l=0; l<numberOfExamples; l++) {
		update(mctm, inputs[l], outputs[l]);
	}
}

void fit(MultiClassTsetlinMachine* mctm, int inputs[][FEATURES], int outputs[], int numberOfExamples, int epochs) {
	for(int epoch=0; epoch<epochs; epoch++) {
		fit(mctm, inputs, outputs, numberOfExamples);
	}
}

#endif