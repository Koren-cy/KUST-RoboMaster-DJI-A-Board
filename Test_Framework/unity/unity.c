/* ==========================================
    Unity Test Framework - Implementation
    Minimal host-side test framework for C.
    No setjmp/longjmp to avoid Windows crashes.
 ========================================== */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "unity.h"

/* ==========================================
 * Global state
 * ========================================== */
const char *Unity_TestFile = NULL;
unsigned int Unity_TestLine = 0;
unsigned int Unity_CurrentTestFailed = 0;
const char *Unity_CurrentTest = NULL;
const char *Unity_CurrentGroup = NULL;
const char *Unity_TestGroup = NULL;
unsigned int Unity_TestNumber = 0;
unsigned int Unity_TestCount = 0;
unsigned int Unity_Failures = 0;
unsigned int Unity_Ignores = 0;
unsigned int Unity_Executions = 0;
unsigned int Unity_Verbose = 0;
static unsigned int Unity_AbortFlag = 0;

/* ==========================================
 * Forward declarations
 * ========================================== */
static void UnityPrintFailedTest(const char *file, unsigned int line);

/* ==========================================
 * Core functions
 * ========================================== */

void UnityTestStart(const char *file, unsigned int line) {
    Unity_TestFile = file;
    Unity_TestLine = line;
    Unity_CurrentTestFailed = 0;
    Unity_AbortFlag = 0;
}

void UnityFail(const char *msg, unsigned int line) {
    UnityTestStart(Unity_TestFile ? Unity_TestFile : __FILE__, line);
    UnityPrint("\n");
    UnityPrint(UnityGetColor(1));
    UnityPrint("FAIL  ");
    UnityPrint(UnityGetColor(0));
    if (msg) {
        UnityPrint("  ");
        UnityPrint(msg);
    }
    UnityPrint("  [");
    if (Unity_CurrentTest) UnityPrint(Unity_CurrentTest);
    UnityPrint("]\n");
    UnityPrint("  at ");
    if (Unity_TestFile) UnityPrint(Unity_TestFile);
    UnityPrint(":");
    {
        char buf[16];
        snprintf(buf, sizeof(buf), "%u", line);
        UnityPrint(buf);
    }
    UnityPrint("\n");
    Unity_CurrentTestFailed = 1;
    Unity_Failures++;
    Unity_AbortFlag = 1;
}

void UnityIgnore(const char *msg, unsigned int line) {
    (void)line;
    UnityPrint("\n");
    UnityPrint(UnityGetColor(2));
    UnityPrint("IGNORE");
    UnityPrint(UnityGetColor(0));
    if (msg) {
        UnityPrint("  ");
        UnityPrint(msg);
    }
    UnityPrint("\n");
    Unity_Ignores++;
    Unity_CurrentTestFailed = 1;
    Unity_AbortFlag = 1;
}

void UnityMessage(const char *msg, unsigned int line) {
    (void)line;
    if (msg) UnityPrint(msg);
}

static void UnityPrintFailedTest(const char *file, unsigned int line) {
    UnityPrint("\n");
    UnityPrint(UnityGetColor(1));
    UnityPrint("FAIL  ");
    UnityPrint(UnityGetColor(0));
    if (file) {
        UnityPrint("at ");
        UnityPrint(file);
        UnityPrint(":");
        {
            char buf[16];
            snprintf(buf, sizeof(buf), "%u", line);
            UnityPrint(buf);
        }
    }
}

/* Helper: print integer */
static void UnityPrintInt(int val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", val);
    UnityPrint(buf);
}

/* Helper: print unsigned integer */
static void UnityPrintUnsigned(unsigned int val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%u", val);
    UnityPrint(buf);
}

/* Helper: print hex */
static void UnityPrintHex(unsigned int val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "0x%X", val);
    UnityPrint(buf);
}

/* ==========================================
 * Integer assertions
 * ========================================== */

void UnityAssertEqualInt(int expected, int actual, const char *msg, unsigned int line) {
    if (expected != actual) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Expected "); UnityPrintInt(expected);
        UnityPrint(" but was "); UnityPrintInt(actual);
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

void UnityAssertNotEqualInt(int expected, int actual, const char *msg, unsigned int line) {
    if (expected == actual) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Values should not be equal: "); UnityPrintInt(expected);
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

void UnityAssertEqualUnsigned(unsigned int expected, unsigned int actual,
                               const char *msg, unsigned int line) {
    if (expected != actual) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Expected "); UnityPrintUnsigned(expected);
        UnityPrint(" but was "); UnityPrintUnsigned(actual);
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

void UnityAssertEqualHex(unsigned int expected, unsigned int actual,
                          const char *msg, unsigned int line) {
    if (expected != actual) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Expected "); UnityPrintHex(expected);
        UnityPrint(" but was "); UnityPrintHex(actual);
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

/* ==========================================
 * String assertions
 * ========================================== */

void UnityAssertEqualString(const char *expected, const char *actual,
                             const char *msg, unsigned int line) {
    if (expected == NULL && actual == NULL) return;
    if (expected == NULL || actual == NULL) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Expected NULL but was ");
        UnityPrint(actual ? actual : "NULL");
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
        return;
    }
    if (strcmp(expected, actual) != 0) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Expected "); UnityPrint(expected);
        UnityPrint(" but was "); UnityPrint(actual);
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

void UnityAssertNotEqualString(const char *expected, const char *actual,
                                const char *msg, unsigned int line) {
    if (expected == NULL && actual == NULL) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Strings should not both be NULL");
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
        return;
    }
    if (expected && actual && strcmp(expected, actual) == 0) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Strings should differ, both were "); UnityPrint(expected);
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

/* ==========================================
 * Pointer assertions
 * ========================================== */

void UnityAssertEqualPointer(const void *expected, const void *actual,
                              const char *msg, unsigned int line) {
    if (expected != actual) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "Expected %p but was %p", expected, actual);
            UnityPrint(buf);
        }
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

void UnityAssertNotEqualPointer(const void *expected, const void *actual,
                                 const char *msg, unsigned int line) {
    if (expected == actual) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "Pointers should differ, both were %p", expected);
            UnityPrint(buf);
        }
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

/* ==========================================
 * Bit assertions
 * ========================================== */

void UnityAssertEqualBits(unsigned int mask, unsigned int expected,
                            unsigned int actual, const char *msg, unsigned int line) {
    if ((expected & mask) != (actual & mask)) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Bits differ within mask ");
        UnityPrintHex(mask);
        UnityPrint(": expected ");
        UnityPrintHex(expected & mask);
        UnityPrint(" but was ");
        UnityPrintHex(actual & mask);
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

/* ==========================================
 * Float assertions
 * ========================================== */

/* Portable NaN check: a value is NaN iff it doesn't equal itself */
#define IS_NAN(f) (((f) != (f)) || ((f) == (f) && 0)) /* simplified: use x != x */

/* Helper to check if float is NaN */
static int float_is_nan(float f) {
    /* NaN != NaN is true */
    return (f != f);
}

void UnityAssertFloatWithin(float delta, float expected, float actual,
                            const char *msg, unsigned int line) {
    float diff = fabsf(expected - actual);

    if (float_is_nan(expected) || float_is_nan(actual)) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Expected NAN but was ");
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "%g", actual);
            UnityPrint(buf);
        }
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
        return;
    }

    if (diff >= delta) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "Expected %g +/- %g but was %g (diff=%g)",
                     expected, delta, actual, diff);
            UnityPrint(buf);
        }
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

void UnityAssertEqualFloatArray(const float *expected, const float *actual,
                                 unsigned int num_elements, float tolerance,
                                 const char *msg, unsigned int line) {
    unsigned int i;
    for (i = 0; i < num_elements; i++) {
        UnityAssertFloatWithin(tolerance, expected[i], actual[i], msg, line);
    }
}

/* ==========================================
 * Double assertions
 * ========================================== */

static int double_is_nan(double d) {
    return (d != d);
}

void UnityAssertDoubleWithin(double delta, double expected, double actual,
                             const char *msg, unsigned int line) {
    double diff = fabs(expected - actual);

    if (double_is_nan(expected) || double_is_nan(actual)) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Expected NAN but was ");
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "%g", actual);
            UnityPrint(buf);
        }
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
        return;
    }

    if (diff >= delta) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "Expected %g +/- %g but was %g",
                     expected, delta, actual);
            UnityPrint(buf);
        }
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

void UnityAssertEqualDoubleArray(const double *expected, const double *actual,
                                  unsigned int num_elements, double tolerance,
                                  const char *msg, unsigned int line) {
    unsigned int i;
    for (i = 0; i < num_elements; i++) {
        UnityAssertDoubleWithin(tolerance, expected[i], actual[i], msg, line);
    }
}

/* ==========================================
 * Raw memory comparison
 * ========================================== */

void UnityAssertNumbersWithin(const void *expected, const void *actual,
                              unsigned int num_bytes, const char *msg,
                              unsigned int line) {
    if (memcmp(expected, actual, num_bytes) != 0) {
        UnityTestStart(Unity_TestFile, line);
        UnityPrintFailedTest(Unity_TestFile, line);
        UnityPrint("Values differ by memory comparison");
        if (msg) { UnityPrint(" - "); UnityPrint(msg); }
        UnityPrint("\n");
        Unity_CurrentTestFailed = 1;
        Unity_Failures++;
        Unity_AbortFlag = 1;
    }
}

/* ==========================================
 * Test runner
 * ========================================== */

void UnityDefaultTestRun(UnityTestFunction func, const char *funcName,
                         unsigned int line) {
    Unity_CurrentTest = funcName;
    Unity_TestLine = line;
    Unity_CurrentTestFailed = 0;
    Unity_AbortFlag = 0;

    func();  /* Run the test body */

    if (!Unity_CurrentTestFailed) {
        UnityPrint(UnityGetColor(0));
        UnityPrint(".");
    }
    /* Note: Unity_Executions is incremented by the test group runner */

    (void)line; /* unused in this simplified runner */
}

unsigned int UnityNumberOfCases(void) {
    return Unity_Executions;
}

/* ==========================================
 * Output utilities
 * ========================================== */

void UnityPrint(const char *msg) {
    if (msg) fputs(msg, stdout);
}

void UnityPrintNumber(int number) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", number);
    UnityPrint(buf);
}

void UnityPrintNumberUnsigned(unsigned int number) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%u", number);
    UnityPrint(buf);
}

void UnityPrintNumberHex(unsigned int number, unsigned int nibbles) {
    char buf[32];
    if (nibbles > 8) nibbles = 8;
    snprintf(buf, sizeof(buf), "0x%0*X", nibbles, number);
    UnityPrint(buf);
}

const char *UnityGetColor(unsigned int result) {
    static const char *colors[] = {
        "\x1B[0m",
        "\x1B[31m",
        "\x1B[33m",
    };
    return colors[result % 3];
}

/* ==========================================
 * Test suite lifecycle
 * ========================================== */

void UnityBegin(const char *filename) {
    Unity_Failures = 0;
    Unity_Ignores = 0;
    Unity_Executions = 0;
    Unity_Verbose = 0;
    Unity_TestFile = filename;
}

int UnityEnd(void) {
    unsigned int total = Unity_Executions;
    unsigned int passed = (total > Unity_Failures) ? (total - Unity_Failures) : 0;

    UnityPrint("\n");
    UnityPrint("========================================\n");
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "  Tests:   %u\n", total); UnityPrint(buf);
        snprintf(buf, sizeof(buf), "  Passes:  %u\n", passed); UnityPrint(buf);
        snprintf(buf, sizeof(buf), "  Failures: %u\n", Unity_Failures); UnityPrint(buf);
        snprintf(buf, sizeof(buf), "  Ignored:  %u\n", Unity_Ignores); UnityPrint(buf);
    }
    UnityPrint("========================================\n");

    if (Unity_Failures == 0) {
        UnityPrint(UnityGetColor(0));
        UnityPrint("ALL TESTS PASSED");
    } else {
        UnityPrint(UnityGetColor(1));
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "%u TEST%s FAILED",
                     Unity_Failures, Unity_Failures > 1 ? "S" : "");
            UnityPrint(buf);
        }
    }
    UnityPrint(UnityGetColor(0));
    UnityPrint("\n\n");
    return (Unity_Failures == 0) ? 0 : 1;
}

void UnityPrintSummary(void) {
    unsigned int total = Unity_Executions;
    unsigned int passed = (total > Unity_Failures) ? (total - Unity_Failures) : 0;
    UnityPrint("\n--------------------------\n");
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "Tests:   %u\n", total); UnityPrint(buf);
        snprintf(buf, sizeof(buf), "Passes:  %u\n", passed); UnityPrint(buf);
        snprintf(buf, sizeof(buf), "Failures: %u\n", Unity_Failures); UnityPrint(buf);
        snprintf(buf, sizeof(buf), "Ignored:  %u\n", Unity_Ignores); UnityPrint(buf);
    }
    UnityPrint("--------------------------\n");
    if (Unity_Failures == 0) {
        UnityPrint(UnityGetColor(0));
        UnityPrint("ALL TESTS PASSED\n");
    } else {
        UnityPrint(UnityGetColor(1));
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "%u TESTS FAILED\n", Unity_Failures);
            UnityPrint(buf);
        }
    }
    UnityPrint(UnityGetColor(0));
}

unsigned int UnityGetFailureCount(void) {
    return Unity_Failures;
}

unsigned int UnityGetIgnoreCount(void) {
    return Unity_Ignores;
}

void UnityConcludeTest(void) {
    /* no-op */
}
