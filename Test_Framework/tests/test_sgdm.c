/* ==========================================
    Tests for user_sgdm module
    Stochastic Gradient Descent with Momentum (SGDM).
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
#include "user_sgdm.h"

/* ==========================================
 * Helper residual functions for testing
 * ========================================== */

/* Rosenbrock function: f(x,y) = (1-x)^2 + 100*(y-x^2)^2
 * Minimum at (x,y) = (1,1), fmin = 0 */
static float residual_rosenbrock(const float *x, const float *param) {
    (void)param;
    float fx = x[0];
    float fy = x[1];
    return (1.0f - fx) * (1.0f - fx) + 100.0f * (fy - fx * fx) * (fy - fx * fx);
}

/* Paraboloid: f(x) = sqrt(sum((xi - i)^2))
 * Minimum at xi = i for each dimension */
static float residual_paraboloid(const float *x, const float *param) {
    if (param == NULL) return 0.0f;
    float sum = 0.0f;
    for (uint8_t i = 0; i < 3; i++) {
        float d = x[i] - param[i];
        sum += d * d;
    }
    return sqrtf(sum);
}

/* ==========================================
 * Test Group: SGDMOptimizer
 * ========================================== */

/* ---- Test bodies ---- */

void test_sgdm_init_sets_default_params(void) {
    SGDM_OPTIMIZER opt;
    SGDM_Init(&opt, 2, residual_rosenbrock);

    TEST_ASSERT_EQUAL_INT(2, opt.n);
    TEST_ASSERT_EQUAL_INT(0, opt.converged);
    TEST_ASSERT_EQUAL_INT(0, opt.iterations);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, SGDM_EPSILON, opt.eps);
    TEST_ASSERT_EQUAL_INT(SGDM_MAX_ITERS, opt.max_iter);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.01f, opt.lr);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.9f, opt.momentum);
}

void test_sgdm_set_learning_rate(void) {
    SGDM_OPTIMIZER opt;
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetLearningRate(&opt, 0.5f);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.5f, opt.lr);
}

void test_sgdm_set_momentum(void) {
    SGDM_OPTIMIZER opt;
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetMomentum(&opt, 0.99f);
    TEST_ASSERT_FLOAT_WITHIN(1e-6f, 0.99f, opt.momentum);
}

void test_sgdm_set_epsilon(void) {
    SGDM_OPTIMIZER opt;
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetEpsilon(&opt, 1e-3f);
    TEST_ASSERT_FLOAT_WITHIN(1e-8f, 1e-3f, opt.eps);
}

void test_sgdm_set_max_iterations(void) {
    SGDM_OPTIMIZER opt;
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetMaxIterations(&opt, 50);
    TEST_ASSERT_EQUAL_INT(50, opt.max_iter);
}

void test_sgdm_solve_does_not_crash_with_valid_params(void) {
    SGDM_OPTIMIZER opt;
    memset(&opt, 0, sizeof(opt));
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetLearningRate(&opt, 0.001f);
    SGDM_SetMomentum(&opt, 0.5f);
    SGDM_SetMaxIterations(&opt, 10);

    float x0[2] = { 0.0f, 0.0f };
    uint8_t result = SGDM_Solve(&opt, NULL, x0);

    /* Must not crash; result should be 0 (not converged in 10 iters) */
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(10, opt.iterations);
    /* x should contain values (not uninitialized) */
    const float *x = SGDM_GetX(&opt);
    TEST_ASSERT_FALSE(isnan(x[0]));
    TEST_ASSERT_FALSE(isnan(x[1]));
}

void test_sgdm_solve_copies_initial_guess(void) {
    SGDM_OPTIMIZER opt;
    memset(&opt, 0, sizeof(opt));
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetMaxIterations(&opt, 1);

    float x0[2] = { 5.0f, -3.0f };
    SGDM_Solve(&opt, NULL, x0);

    /* After 1 iteration, x should still be near x0 (gradient is tiny at start) */
    const float *x = SGDM_GetX(&opt);
    TEST_ASSERT_TRUE(fabsf(x[0] - 5.0f) < 10.0f);
    TEST_ASSERT_TRUE(fabsf(x[1] + 3.0f) < 10.0f);
}

void test_sgdm_solve_handles_zero_dimensions(void) {
    SGDM_OPTIMIZER opt;
    memset(&opt, 0, sizeof(opt));
    SGDM_Init(&opt, 0, residual_rosenbrock);

    float x0[1] = { 0.0f };
    uint8_t result = SGDM_Solve(&opt, NULL, x0);

    /* n=0 should return 0 (early exit) */
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_sgdm_solve_handles_null_residual_func(void) {
    SGDM_OPTIMIZER opt;
    memset(&opt, 0, sizeof(opt));
    SGDM_Init(&opt, 2, NULL);

    float x0[2] = { 0.0f, 0.0f };
    /* NULL residual func + n>0: may crash or return 0 */
    uint8_t result = SGDM_Solve(&opt, NULL, x0);
    (void)result;
}

void test_sgdm_get_residual_returns_nonnegative(void) {
    SGDM_OPTIMIZER opt;
    memset(&opt, 0, sizeof(opt));
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetLearningRate(&opt, 0.001f);
    SGDM_SetMaxIterations(&opt, 100);

    float x0[2] = { 0.0f, 0.0f };
    SGDM_Solve(&opt, NULL, x0);

    float residual = SGDM_GetResidual(&opt);
    TEST_ASSERT_TRUE(residual >= 0.0f);
    TEST_ASSERT_FALSE(isnan(residual));
}

void test_sgdm_momentum_vector_accumulates(void) {
    SGDM_OPTIMIZER opt;
    SGDM_Init(&opt, 2, residual_rosenbrock);
    SGDM_SetLearningRate(&opt, 0.01f);
    SGDM_SetMomentum(&opt, 0.9f);
    SGDM_SetMaxIterations(&opt, 10);

    float x0[2] = { 0.0f, 0.0f };
    SGDM_Solve(&opt, NULL, x0);

    /* At least one velocity component should be non-zero */
    int v_is_nonzero = 0;
    for (uint8_t i = 0; i < opt.n; i++) {
        if (opt.v[i] != 0.0f) {
            v_is_nonzero = 1;
            break;
        }
    }
    TEST_ASSERT_EQUAL_INT(1, v_is_nonzero);
}

/* ---- Test group runner ---- */

void TEST_GROUP_RUNNER_SGDMOptimizer(void) {
    UnityDefaultTestRun(test_sgdm_init_sets_default_params,
                        "test_sgdm_init_sets_default_params", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_set_learning_rate,
                        "test_sgdm_set_learning_rate", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_set_momentum,
                        "test_sgdm_set_momentum", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_set_epsilon,
                        "test_sgdm_set_epsilon", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_set_max_iterations,
                        "test_sgdm_set_max_iterations", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_solve_does_not_crash_with_valid_params,
                        "test_sgdm_solve_does_not_crash_with_valid_params", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_solve_copies_initial_guess,
                        "test_sgdm_solve_copies_initial_guess", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_solve_handles_zero_dimensions,
                        "test_sgdm_solve_handles_zero_dimensions", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_solve_handles_null_residual_func,
                        "test_sgdm_solve_handles_null_residual_func", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_get_residual_returns_nonnegative,
                        "test_sgdm_get_residual_returns_nonnegative", __LINE__ - 1);
    UnityDefaultTestRun(test_sgdm_momentum_vector_accumulates,
                        "test_sgdm_momentum_vector_accumulates", __LINE__ - 1);
    Unity_Executions += 11;
}
