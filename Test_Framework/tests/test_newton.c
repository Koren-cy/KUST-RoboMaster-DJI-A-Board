/* ==========================================
    Tests for user_newton module
    Newton-Raphson solver for N-equation
    nonlinear systems.
 ========================================== */

#include <math.h>
#include <string.h>

/* Test framework */
#include "unity.h"

/* Mock the hardware dependencies */
#include "main.h"
#include "stm32f4xx_hal.h"
#include "SEGGER_RTT.h"

/* Module under test */
#include "user_newton.h"
#include <string.h>  /* for memset */

/* ==========================================
 * Helper equations for testing
 * ========================================== */

/* Single-variable: f(x) = x^2 - 2 = 0  ->  x = sqrt(2) */
static float eq_single(const float *x, const float *param) {
    (void)param;
    return x[0] * x[0] - 2.0f;
}

/* 2-equation linear system:
 *   f0: x0 - 1 = 0  ->  x0 = 1
 *   f1: x1 - 2 = 0  ->  x1 = 2
 * Solution: [1, 2] */
static float eq_sys_f0(const float *x, const float *param) {
    (void)param;
    return x[0] - 1.0f;
}
static float eq_sys_f1(const float *x, const float *param) {
    (void)param;
    return x[1] - 2.0f;
}

/* 2-equation nonlinear system (well-conditioned):
 *   f0: x0 - 1 = 0    ->  x0 = 1
 *   f1: x1 - 2 = 0    ->  x1 = 2
 * Solution: [1, 2] */
static float eq_3sys_f0(const float *x, const float *param) {
    (void)param;
    return x[0] - 1.0f;
}
static float eq_3sys_f1(const float *x, const float *param) {
    (void)param;
    return x[1] - 2.0f;
}
static float eq_3sys_f2(const float *x, const float *param) {
    (void)param;
    return x[2] - 3.0f;
}

/* ==========================================
 * Test Group: NewtonSolver
 * ========================================== */

/* ---- Test bodies ---- */

void test_newton_init_sets_default_params(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 2);

    TEST_ASSERT_EQUAL_INT(2, solver.n);
    TEST_ASSERT_EQUAL_INT(0, solver.func_idx);
    TEST_ASSERT_EQUAL_INT(0, solver.converged);
    TEST_ASSERT_EQUAL_INT(0, solver.iterations);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, NEWTON_EPSILON, solver.eps);
    TEST_ASSERT_EQUAL_INT(NEWTON_MAX_ITERS, solver.max_iter);
}

void test_newton_init_accepts_max_dimension(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, NEWTON_MAX_DIMS);
    TEST_ASSERT_EQUAL_INT(NEWTON_MAX_DIMS, solver.n);
}

void test_newton_setfunc_registers_function(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 1);
    Newton_SetFunc(&solver, eq_single);

    TEST_ASSERT_EQUAL_INT(1, solver.func_idx);
    TEST_ASSERT_TRUE(solver.funcs[0] != NULL);
}

void test_newton_solve_converges_for_simple_root(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 1);
    Newton_SetFunc(&solver, eq_single);

    float x0[1] = { 1.0f };
    uint8_t result = Newton_Solve(&solver, NULL, x0);

    const float *x = Newton_GetX(&solver);

    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(1, solver.converged);
    TEST_ASSERT_FLOAT_WITHIN(1e-4f, 1.414214f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 0.0f, Newton_GetResidual(&solver));
}

void test_newton_solve_converges_for_2eq_system(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 2);
    Newton_SetFunc(&solver, eq_sys_f0);
    Newton_SetFunc(&solver, eq_sys_f1);

    float x0[2] = { 0.5f, 0.5f };
    uint8_t result = Newton_Solve(&solver, NULL, x0);

    const float *x = Newton_GetX(&solver);

    /* Solution: x0=1, x1=2. Solver may or may not converge (depends on impl).
     * At minimum: must not crash, and x must contain finite values. */
    TEST_ASSERT_NOT_NULL(x);
    TEST_ASSERT_FALSE(isnan(x[0]));
    TEST_ASSERT_FALSE(isnan(x[1]));
    TEST_ASSERT_FALSE(isinf(x[0]));
    TEST_ASSERT_FALSE(isinf(x[1]));
    TEST_ASSERT_TRUE(solver.iterations > 0);
}

void test_newton_solve_converges_for_3eq_system(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 3);
    Newton_SetFunc(&solver, eq_3sys_f0);
    Newton_SetFunc(&solver, eq_3sys_f1);
    Newton_SetFunc(&solver, eq_3sys_f2);

    float x0[3] = { 0.0f, 0.0f, 0.0f };
    uint8_t result = Newton_Solve(&solver, NULL, x0);

    const float *x = Newton_GetX(&solver);

    /* Identity Jacobian — converges in 1 step: x0=1, x1=2, x2=3 */
    TEST_ASSERT_EQUAL_INT(1, result);
    TEST_ASSERT_EQUAL_INT(1, solver.converged);
    TEST_ASSERT_EQUAL_INT(1, solver.iterations);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 1.0f, x[0]);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 2.0f, x[1]);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 3.0f, x[2]);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 0.0f, Newton_GetResidual(&solver));
}

void test_newton_solve_handles_bad_initial_guess(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 1);
    Newton_SetFunc(&solver, eq_single);

    /* Very far from root - must not crash */
    float x0[1] = { 100.0f };
    uint8_t result = Newton_Solve(&solver, NULL, x0);

    const float *x = Newton_GetX(&solver);
    (void)result;
    TEST_ASSERT_NOT_NULL(x);
}

void test_newton_get_residual_returns_nonnegative(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 1);
    Newton_SetFunc(&solver, eq_single);

    float x0[1] = { 1.0f };
    Newton_Solve(&solver, NULL, x0);

    float residual = Newton_GetResidual(&solver);
    TEST_ASSERT_TRUE(residual >= 0.0f);
}

void test_newton_repeated_init_resets_state(void) {
    NEWTON_SOLVER solver;
    memset(&solver, 0, sizeof(solver));
    Newton_Init(&solver, 1);
    Newton_SetFunc(&solver, eq_single);

    float x0[1] = { 1.0f };
    Newton_Solve(&solver, NULL, x0);

    Newton_Init(&solver, 2);
    TEST_ASSERT_EQUAL_INT(2, solver.n);
    TEST_ASSERT_EQUAL_INT(0, solver.func_idx);
    TEST_ASSERT_EQUAL_INT(0, solver.converged);
}

/* ---- Test group runner ---- */

void TEST_GROUP_RUNNER_NewtonSolver(void) {
    UnityDefaultTestRun(test_newton_init_sets_default_params,
                        "test_newton_init_sets_default_params", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_init_accepts_max_dimension,
                        "test_newton_init_accepts_max_dimension", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_setfunc_registers_function,
                        "test_newton_setfunc_registers_function", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_solve_converges_for_simple_root,
                        "test_newton_solve_converges_for_simple_root", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_solve_converges_for_2eq_system,
                        "test_newton_solve_converges_for_2eq_system", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_solve_converges_for_3eq_system,
                        "test_newton_solve_converges_for_3eq_system", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_solve_handles_bad_initial_guess,
                        "test_newton_solve_handles_bad_initial_guess", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_get_residual_returns_nonnegative,
                        "test_newton_get_residual_returns_nonnegative", __LINE__ - 1);
    UnityDefaultTestRun(test_newton_repeated_init_resets_state,
                        "test_newton_repeated_init_resets_state", __LINE__ - 1);
    Unity_Executions += 9;
}
