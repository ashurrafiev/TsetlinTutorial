#ifndef _TSETLIN_MACHINE_H_
#define _TSETLIN_MACHINE_H_

#include <stdlib.h>

#include "TsetlinOptions.h"

#define LITERALS (FEATURES*2)

#define PROMOTE (+1)
#define DEMOTE (-1)

struct Clause {
	// TA states for positive (even) and negative (odd) polarity literals
	int ta[LITERALS];
	// clause output (cached value)
	int output;
};

struct TsetlinMachine { 
	Clause clauses[CLAUSES];
};

/**
 * Randomly returns true with the given probability
 */
#define WITH_PROBABILITY(prob) (rand() < (int)((prob)*RAND_MAX))

/**
 * Returns voting sign for j-th clause: odd clauses vote -1, even clauses vote +1
 */
#define VOTE(j) (((j)&1) ? -1 : 1)

/**
 * Returns polarity of k-th literal: odd literals are negated (1), even literals are positive (0)
 */
#define POLARITY(k) ((k)&1)

/**
 * Calculate the value of k-th literal on the given input respecting literal polarity
 */
#define LITERAL_VALUE(input, k) ((input)[(k)/2] ^ POLARITY(k))

/**
 * Determine include (1) or exclude (0) decision based on a TA state
 */
#define INCLUDE_LITERAL(state) ((state) > 0)

int calculateOutput(Clause* clause, int input[], int eval) {
	clause->output = 1;
	int inc = 0;
	// calculate conjunction over k literals
	// (we can stop early if output becomes false)
	for(int k=0; clause->output && k<LITERALS; k++) {
		if(INCLUDE_LITERAL(clause->ta[k])) {
			clause->output &= LITERAL_VALUE(input, k);
			inc = 1;
		}
	}
	if(eval && !inc)
		return clause->output = 0;
	return clause->output;
}

inline void updateTA(int* ta, int action) {
	int nextState = (*ta)+action;
	
	// update, if next state is within allowed states
	if(nextState>-NUM_STATES && nextState<=NUM_STATES)
		(*ta) = nextState;
}

/*
void reward(Clause* clause, int k) {
	if(INCLUDE_LITERAL(clause->ta[k]))
		updateTA(&clause->ta[k], +1 );
	else
		updateTA(&clause->ta[k], -1 );
}

void penalty(Clause* clause, int k) {
	if(INCLUDE_LITERAL(clause->ta[k]))
		updateTA(&clause->ta[k], -1 );
	else
		updateTA(&clause->ta[k], +1 );
}
*/

void initialize(TsetlinMachine* tm) {
	for(int j=0; j<CLAUSES; j++) {				
		// set initial TA states to random noise
		for(int k=0; k<LITERALS; k+=2) {
			if(WITH_PROBABILITY(0.5)) {
				tm->clauses[j].ta[k] = 1;
				tm->clauses[j].ta[k+1] = 0; 
			}
			else {
				tm->clauses[j].ta[k] = 0;
				tm->clauses[j].ta[k+1] = 1;
			}
		}
	}
}
	
TsetlinMachine* createTsetlinMachine() {
	struct TsetlinMachine *tm = (TsetlinMachine *)malloc(sizeof(struct TsetlinMachine));
	initialize(tm);
	return tm;
}

/**
 * Update clauses for the given input vector
 * @param input
 */
void calculateClauseOutputs(TsetlinMachine* tm, int input[], int eval) {
	for(int j=0; j<CLAUSES; j++) {
		calculateOutput(&tm->clauses[j], input, eval);
	}
}

/**
 * Calculate class voting based on the clause outputs.
 * Must be called after calculateClauseOuputs.
 */
int calculateVoting(TsetlinMachine* tm) {
	int sum = 0;
	for(int j=0; j<CLAUSES; j++) {
		// if output is true, the clause is active
		if(tm->clauses[j].output) {
			// add vote
			sum += VOTE(j);
		}
	}
	if(sum<-L_THRESHOLD)
		return -L_THRESHOLD;
	else if(sum>L_THRESHOLD)
		return L_THRESHOLD;
	else
		return sum;
}

void typeIFeedbackLiteral(int k, Clause* clause, int literalValue) {
	if(clause->output && literalValue) { // clause is 1 and literal is 1
		if(WITH_PROBABILITY(1.0-1.0/L_RATE))
			updateTA(&clause->ta[k], PROMOTE);
	}
	else { // clause is 0 or literal is 0
		if(WITH_PROBABILITY(1.0/L_RATE))
			updateTA(&clause->ta[k], DEMOTE);
	}
}

void typeIFeedback(Clause* clause, int input[]) {
	for(int k=0; k<LITERALS; k++) {
		typeIFeedbackLiteral(k, clause, LITERAL_VALUE(input, k));
	}
}

void typeIIFeedbackLiteral(int k, Clause* clause, int literalValue) {
	if(!literalValue && !INCLUDE_LITERAL(clause->ta[k])) // if literal is 0 and excluded
		updateTA(&clause->ta[k], PROMOTE);
}

void typeIIFeedback(Clause* clause, int input[]) {
	// only if clause is 1
	if(clause->output) {
		for(int k=0; k<LITERALS; k++) {
			typeIIFeedbackLiteral(k, clause, LITERAL_VALUE(input, k));
		}
	}
}

void update(TsetlinMachine* tm, int input[], int output) {
	calculateClauseOutputs(tm, input, 0);
	int classSum = calculateVoting(tm);
	
	for(int j=0; j<CLAUSES; j++) {
		// inverse the decision for negatively-voting clauses
		int y;
		if(VOTE(j)>0)
			y = output;
		else
			y = !output;

		// calculate feedback probability
		double feedbackProbability;
		if(y) {
			feedbackProbability = (L_THRESHOLD - classSum) / (2.0 * L_THRESHOLD);
			if(WITH_PROBABILITY(feedbackProbability))
				typeIFeedback(&tm->clauses[j], input);
		}
		else {
			feedbackProbability = (L_THRESHOLD + classSum) / (2.0 * L_THRESHOLD);
			if(WITH_PROBABILITY(feedbackProbability))
				typeIIFeedback(&tm->clauses[j], input);
		}
	}
}

int score(TsetlinMachine* tm, int input[]) {
	calculateClauseOutputs(tm, input, 1);
	return calculateVoting(tm);
}

#endif

