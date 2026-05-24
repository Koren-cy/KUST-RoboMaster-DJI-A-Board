#include <stdio.h>
#include <string.h>
#include "unity.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "SEGGER_RTT.h"
#include "user_newton.h"

static float eq_f0(const float *x, const float *param) {
    (void)param;
    return x[0] - 1.0f;
}
static float eq_f1(const float *x, const float *param) {
    (void)param;
    return x[1] - 2.0f;
}
static float eq_f2(const float *x, const float *param) {
    (void)param;
    return x[0] + x[1] - 3.0f;
}

void test_3eq_system(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 3);
    Newton_SetFunc(&solver, eq_f0);
    Newton_SetFunc(&solver, eq_f1);
    Newton_SetFunc(&solver, eq_f2);

    float x0[3] = { 0.0f, 0.0f, 0.0f };
    uint8_t result = Newton_Solve(&solver, NULL, x0);

    fprintf(stderr, "3-eq test:\n");
    fprintf(stderr, "  result=%d\n", result);
    fprintf(stderr, "  iterations=%d\n", solver.iterations);
    fprintf(stderr, "  converged=%d\n", solver.converged);
    fprintf(stderr, "  residual=%e\n", (double)solver.residual);
    const float *x = Newton_GetX(&solver);
    fprintf(stderr, "  x={%e, %e, %e}\n",
            (double)x[0], (double)x[1], (double)x[2]);

    TEST_ASSERT_EQUAL_INT(3, solver.n);
}

void TEST_GROUP_RUNNER_NewtonSolver(void) {
    UnityDefaultTestRun(test_3eq_system, "test_3eq_system", __LINE__ - 1);
    Unity_Executions += 1;
}

int main(void) {
    fprintf(stderr, "START\n"); fflush(stderr);
    UnityBegin(__FILE__);
    fprintf(stderr, "After UnityBegin\n"); fflush(stderr);
    TEST_GROUP_RUNNER_NewtonSolver();
    fprintf(stderr, "After test runner\n"); fflush(stderr);
    UnityPrintSummary();
    fprintf(stderr, "ALL DONE\n"); fflush(stderr);
    return 0;
}
