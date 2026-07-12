#ifndef OUTCOME_H
#define OUTCOME_H

enum Outcome {
    OUTCOME_OK,             /* thumbs up; everything went well */
    OUTCOME_E601_ZERO_DIVISION,
    OUTCOME_E604_INDEX
};

typedef enum Outcome    Outcome;

#endif
