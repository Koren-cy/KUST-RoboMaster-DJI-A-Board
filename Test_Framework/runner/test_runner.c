/* ==========================================
    Test Runner - Main Entry Point
    Runs all registered TEST_GROUPs.
    To add a new test group:
      1. Declare: extern TEST_GROUP_RUNNER_DECLARE(MyGroup);
      2. Call:    RUN_TEST_GROUP(MyGroup); in main()
 ========================================== */

#include <stdio.h>
#include <stdlib.h>

/* Unity framework */
#include "unity.h"

/* ---- Declare all test group runners ---- */
TEST_GROUP_RUNNER_DECLARE(NewtonSolver);
TEST_GROUP_RUNNER_DECLARE(SGDMOptimizer);

/* ==========================================
 * Entry point
 * ========================================== */
int main(void) {
    Unity_TestFile = __FILE__;

    printf("======================================\n");
    printf("  RoboMaster DJI A-Board Test Suite\n");
    printf("======================================\n\n");

    /* Run Newton Solver tests */
    printf("[ NewtonSolver ]\n");
    RUN_TEST_GROUP(NewtonSolver);
    printf("\n");

    /* Run SGDM Optimizer tests */
    printf("[ SGDMOptimizer ]\n");
    RUN_TEST_GROUP(SGDMOptimizer);
    printf("\n");

    /* Print summary */
    UnityPrintSummary();

    if (UnityGetFailureCount() > 0) {
        return 1;
    }
    return 0;
}
