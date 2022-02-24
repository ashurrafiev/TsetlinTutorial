#ifndef _TSETLIN_OPTIONS_H_
#define _TSETLIN_OPTIONS_H_

#define CLAUSES 100
#define NUM_STATES 100
#define L_RATE 10.0
#define L_THRESHOLD 20
#define FEATURES (28*28)
#define CLASSES 10

#define NUM_EXAMPLES_TRAIN 60000
#define NUM_EXAMPLES_TEST 10000

#define INPUT_DATA_PATH "../../poets/tsetlin"

#define RAND_SEED 0
#define ACC_EVAL_TRAIN 0
#define ACC_EVAL_TEST 6

#define LOG_TASTATES 1
#define LOG_STATUS 1
#define LOG_APPEND 1

#define LOAD_STATE "state0.csv"
#define REMAP_STATE 1
#define SAVE_STATE "state1.csv"

#endif
