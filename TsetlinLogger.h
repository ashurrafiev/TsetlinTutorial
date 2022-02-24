#ifndef _TSETLIN_LOGGER_H_
#define _TSETLIN_LOGGER_H_

#include "MultiClassTsetlinMachine.h"

#define TASTATES_PATH "tm-spectrum.csv"
#define STATUS_PATH "tm-status.csv"

#define MIN_STATE (-NUM_STATES+1)
#define MAX_STATE (NUM_STATES)
#define ABS_STATES (NUM_STATES*2)

struct LogTAStates {
	FILE* fp;
};

void startLogTAStates(LogTAStates* log) {
	#if LOG_TASTATES
	#if LOG_APPEND
		log->fp = fopen(TASTATES_PATH, "at");
	#else
		log->fp = fopen(TASTATES_PATH, "wt");
	#endif
	if (log->fp == NULL) {
		printf("Error writing %s\n", TASTATES_PATH);
		exit(EXIT_FAILURE);
	}
	#if !LOG_APPEND
		fprintf(log->fp, "t\t");
		for(int i=0; i<CLASSES; i++) {
			for(int s=MIN_STATE; s<=MAX_STATE; s++) {
				fprintf(log->fp, "c%d%+d\t", i, s);
			}
		}
		fprintf(log->fp, "\n");
		fflush(log->fp);
	#endif
	#endif
}

void logTAStates(LogTAStates* log, int step, MultiClassTsetlinMachine* mctm) {
	#if LOG_TASTATES
	fprintf(log->fp, "%d\t", step);
	int counts[ABS_STATES];
	
	for(int i=0; i<CLASSES; i++) {
		for(int s=0; s<ABS_STATES; s++) {
			counts[s] = 0;
		}
		TsetlinMachine* tm = &mctm->tsetlinMachines[i];
		for(int j=0; j<CLAUSES; j++) {				
			for(int k=0; k<LITERALS; k++) {
				counts[tm->clauses[j].ta[k] + NUM_STATES - 1]++;
			}
		}
		for(int s=0; s<ABS_STATES; s++) {
			fprintf(log->fp, "%d\t", counts[s]);
		}
	}
	
	fprintf(log->fp, "\n");
	fflush(log->fp);
	#endif
}

void finishLogTAStates(LogTAStates* log) {
	#if LOG_TASTATES
	fclose(log->fp);
	#endif
}

struct LogStatus {
	FILE* fp;
	float accTrain, accTest;
};

void startLogStatus(LogStatus* log) {
	#if LOG_STATUS
	#if LOG_APPEND
		log->fp = fopen(STATUS_PATH, "at");
	#else
		log->fp = fopen(STATUS_PATH, "wt");
	#endif
	if (log->fp == NULL) {
		printf("Error writing %s\n", STATUS_PATH);
		exit(EXIT_FAILURE);
	}
	
	#if !LOG_APPEND
		fprintf(log->fp, "t\t");
		fprintf(log->fp, "acctrain\t");
		fprintf(log->fp, "acctest\t");
		for(int i=0; i<CLASSES; i++) {
			fprintf(log->fp, "inc%d\t", i);
			fprintf(log->fp, "flips%d\t", i);
			fprintf(log->fp, "type1-c%d\t", i);
			fprintf(log->fp, "type2-c%d\t", i);
		}
		fprintf(log->fp, "\n");
		fflush(log->fp);
	#endif
	
	log->accTrain = 0;
	log->accTest = 0;
	#endif
}

void logStatus(LogStatus* log, int step, MultiClassTsetlinMachine* mctm) {
	#if LOG_STATUS
	fprintf(log->fp, "%d\t", step);
	
	fprintf(log->fp, "%.3f\t", log->accTrain);
	fprintf(log->fp, "%.3f\t", log->accTest);
	for(int i=0; i<CLASSES; i++) {
		TsetlinMachine* tm = &mctm->tsetlinMachines[i];
		fprintf(log->fp, "%d\t", countIncluded(tm));
		fprintf(log->fp, "%d\t", tm->flips);
		tm->flips = 0;
		fprintf(log->fp, "%d\t", tm->countType1);
		tm->countType1 = 0;
		fprintf(log->fp, "%d\t", tm->countType2);
		tm->countType2 = 0;
	}
	
	fprintf(log->fp, "\n");
	fflush(log->fp);
	#endif
}

void finishLogStatus(LogStatus* log) {
	#if LOG_STATUS
	fclose(log->fp);
	#endif
}


#endif
