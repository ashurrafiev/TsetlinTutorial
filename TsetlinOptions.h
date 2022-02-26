#ifndef _TSETLIN_OPTIONS_H_
#define _TSETLIN_OPTIONS_H_

#define CLAUSES 100
#define NUM_STATES 100
#define L_RATE 5.0
#define L_NORM_THRESHOLD 0.35
#define FEATURES (28*28)
#define CLASSES 10

#define L_THRESHOLD ((int)(L_NORM_THRESHOLD*CLAUSES/2))

#define NUM_EXAMPLES_TRAIN 60000
#define NUM_EXAMPLES_TEST 10000

#define INPUT_DATA_PATH "../../poets/tsetlin"

#define RAND_SEED 0
#define ACC_EVAL_TRAIN 0
#define ACC_EVAL_TEST -1

#define LOG_TASTATES 0
#define LOG_STATUS 0
#define LOG_APPEND 0

//#define LOAD_STATE "state0.csv"
//#define REMAP_STATE 1
//#define SAVE_STATE "state1.csv"

#endif
