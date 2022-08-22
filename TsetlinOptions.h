#ifndef _TSETLIN_OPTIONS_H_
#define _TSETLIN_OPTIONS_H_

#define CLAUSES 200
#define NUM_STATES 100

double L_RATE = 5.0;
double L_NORM_THRESHOLD = 0.35;
#define L_THRESHOLD (L_NORM_THRESHOLD*CLAUSES/2.0)
#define FIX_FEEDBACK_PROB 1

#define FEATURES (28*28)
#define CLASSES 10
#define NUM_EXAMPLES_TRAIN 60000
#define NUM_EXAMPLES_TEST 10000

#define INPUT_DATA_PATH "../../poets/tsetlin"

int RAND_SEED = 0;
int ACC_EVAL_TRAIN = 0;
int ACC_EVAL_TEST = 0;
int ACC_EVAL_CLASSES = 0;

#define LOG_ENABLED 1

#if LOG_ENABLED
	int LOG_TASTATES = 0;
	int LOG_STATUS = 0;
	int LOG_APPEND = 0;

	int LOAD_STATE = 0;
	char LOAD_STATE_PATH[1024] = "in_state.csv";
	int REMAP_STATE = 0;
	int SAVE_STATE = 0;
	char SAVE_STATE_PATH[1024] = "out_state.csv";
#endif

#endif
