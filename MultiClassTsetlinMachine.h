#ifndef _MULTI_CLASS_TSETLIN_MACHINE_H_
#define _MULTI_CLASS_TSETLIN_MACHINE_H_

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

double evaluate(MultiClassTsetlinMachine* mctm, int inputs[][FEATURES], int labels[], int numberOfExamples) {
	int maxClass;
	int maxClassSum;

	int errors = 0;
	for(int l=0; l<numberOfExamples; l++) {
		maxClassSum = score(&mctm->tsetlinMachines[0], inputs[l]);
		maxClass = 0;
		for(int i=1; i<CLASSES; i++) {	
			int classSum = score(&mctm->tsetlinMachines[i], inputs[l]);
			if(maxClassSum < classSum) {
				maxClassSum = classSum;
				maxClass = i;
			}
		}

		if(maxClass!=labels[l]) {
			errors += 1;
		}
	}
	
	return 1.0 - errors / (double)numberOfExamples;
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