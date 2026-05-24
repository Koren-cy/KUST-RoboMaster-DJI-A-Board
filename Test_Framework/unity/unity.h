/* ==========================================
    Unity Test Framework - Host-Side Build
    Based on ThrowTheSwitch/Unity (MIT License).
    Adapted for clean host-side compilation.
 ========================================== */

#ifndef UNITY_H_
#define UNITY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/* ==========================================
 * Unity version
 * ========================================== */
#define UNITY_VERSION_MAJOR  2
#define UNITY_VERSION_MINOR  5
#define UNITY_VERSION_PATCH  2
#define UNITY_VERSION       (UNITY_VERSION_MAJOR * 1000 + UNITY_VERSION_MINOR * 100 + UNITY_VERSION_PATCH)

/* ==========================================
 * Configuration
 * ========================================== */
#ifndef UNITY_TIMEOUT
#define UNITY_TIMEOUT 1000
#endif
#ifndef UNITY_MAX_EXAMPLES
#define UNITY_MAX_EXAMPLES 64
#endif
#ifndef UNITY_FLOAT_PRECISION
#define UNITY_FLOAT_PRECISION 0.000001f
#endif
#ifndef UNITY_DOUBLE_PRECISION
#define UNITY_DOUBLE_PRECISION 0.0000000001
#endif
#ifndef UNITY_INTERNAL_FLOAT_TOOL
#define UNITY_INTERNAL_FLOAT_TOOL 1e-9f
#endif

/* ==========================================
 * Unity global state (plain identifiers)
 * ========================================== */
extern const char *Unity_TestFile;
extern unsigned int Unity_TestLine;
extern unsigned int Unity_CurrentTestFailed;
extern const char *Unity_CurrentTest;
extern const char *Unity_CurrentGroup;
extern const char *Unity_TestGroup;
extern unsigned int Unity_TestNumber;
extern unsigned int Unity_TestCount;
extern unsigned int Unity_Failures;
extern unsigned int Unity_Ignores;
extern unsigned int Unity_Executions;
extern unsigned int Unity_Verbose;

/* ==========================================
 * Function declarations
 * ========================================== */
void UnityBegin(const char *filename);
int  UnityEnd(void);
void UnityFail(const char *msg, unsigned int line);
void UnityIgnore(const char *msg, unsigned int line);
void UnityMessage(const char *msg, unsigned int line);

/* ==========================================
 * Core assertion macros
 * ========================================== */
#define TEST_ASSERT(condition) do { \
    if (!(condition)) UnityFail("Expression evaluated to false: " #condition, __LINE__); \
} while (0)

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT(condition)
#define TEST_ASSERT_FALSE(condition) TEST_ASSERT(!(condition))
#define TEST_ASSERT_TRUE_MESSAGE(condition, msg) do { \
    if (!(condition)) UnityFail(msg, __LINE__); \
} while (0)
#define TEST_ASSERT_FALSE_MESSAGE(condition, msg) do { \
    if (!!(condition)) UnityFail(msg, __LINE__); \
} while (0)

#define TEST_FAIL_MESSAGE(msg) UnityFail(msg, __LINE__)
#define TEST_FAIL() UnityFail("Test Failed", __LINE__)
#define TEST_IGNORE_MESSAGE(msg) UnityIgnore(msg, __LINE__)
#define TEST_IGNORE() UnityIgnore("Test Ignored", __LINE__)

#define TEST_ASSERT_NULL(pointer) do { \
    if ((pointer) != NULL) UnityFail("Expected NULL, saw non-NULL", __LINE__); \
} while (0)

#define TEST_ASSERT_NOT_NULL(pointer) do { \
    if ((pointer) == NULL) UnityFail("Expected non-NULL, saw NULL", __LINE__); \
} while (0)

/* ==========================================
 * Integer assertions
 * ========================================== */
#define TEST_ASSERT_EQUAL_INT(expected, actual) \
    UnityAssertEqualInt((int)(expected), (int)(actual), NULL, __LINE__)

#define TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, msg) \
    UnityAssertEqualInt((int)(expected), (int)(actual), (msg), __LINE__)

#define TEST_ASSERT_NOT_EQUAL_INT(expected, actual) \
    UnityAssertNotEqualInt((int)(expected), (int)(actual), NULL, __LINE__)

#define TEST_ASSERT_NOT_EQUAL_INT_MESSAGE(expected, actual, msg) \
    UnityAssertNotEqualInt((int)(expected), (int)(actual), (msg), __LINE__)

#define TEST_ASSERT_EQUAL_INT8(expected, actual) \
    UnityAssertEqualInt((int)(int8_t)(expected), (int)(int8_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_INT16(expected, actual) \
    UnityAssertEqualInt((int)(int16_t)(expected), (int)(int16_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_INT32(expected, actual) \
    UnityAssertEqualInt((int)(int32_t)(expected), (int)(int32_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_INT8_MESSAGE(expected, actual, msg) \
    UnityAssertEqualInt((int)(int8_t)(expected), (int)(int8_t)(actual), (msg), __LINE__)
#define TEST_ASSERT_EQUAL_INT16_MESSAGE(expected, actual, msg) \
    UnityAssertEqualInt((int)(int16_t)(expected), (int)(int16_t)(actual), (msg), __LINE__)
#define TEST_ASSERT_EQUAL_INT32_MESSAGE(expected, actual, msg) \
    UnityAssertEqualInt((int)(int32_t)(expected), (int)(int32_t)(actual), (msg), __LINE__)

#define TEST_ASSERT_EQUAL_UINT(expected, actual) \
    UnityAssertEqualUnsigned((unsigned int)(expected), (unsigned int)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_UINT_MESSAGE(expected, actual, msg) \
    UnityAssertEqualUnsigned((unsigned int)(expected), (unsigned int)(actual), (msg), __LINE__)

#define TEST_ASSERT_EQUAL_UINT8(expected, actual) \
    UnityAssertEqualUnsigned((unsigned int)(uint8_t)(expected), (unsigned int)(uint8_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_UINT16(expected, actual) \
    UnityAssertEqualUnsigned((unsigned int)(uint16_t)(expected), (unsigned int)(uint16_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_UINT32(expected, actual) \
    UnityAssertEqualUnsigned((unsigned int)(uint32_t)(expected), (unsigned int)(uint32_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_UINT8_MESSAGE(expected, actual, msg) \
    UnityAssertEqualUnsigned((unsigned int)(uint8_t)(expected), (unsigned int)(uint8_t)(actual), (msg), __LINE__)
#define TEST_ASSERT_EQUAL_UINT16_MESSAGE(expected, actual, msg) \
    UnityAssertEqualUnsigned((unsigned int)(uint16_t)(expected), (unsigned int)(uint16_t)(actual), (msg), __LINE__)
#define TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, actual, msg) \
    UnityAssertEqualUnsigned((unsigned int)(uint32_t)(expected), (unsigned int)(uint32_t)(actual), (msg), __LINE__)

#define TEST_ASSERT_EQUAL_HEX8(expected, actual) \
    UnityAssertEqualHex((unsigned int)(uint8_t)(expected), (unsigned int)(uint8_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_HEX16(expected, actual) \
    UnityAssertEqualHex((unsigned int)(uint16_t)(expected), (unsigned int)(uint16_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_HEX32(expected, actual) \
    UnityAssertEqualHex((unsigned int)(uint32_t)(expected), (unsigned int)(uint32_t)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_HEX8_MESSAGE(expected, actual, msg) \
    UnityAssertEqualHex((unsigned int)(uint8_t)(expected), (unsigned int)(uint8_t)(actual), (msg), __LINE__)
#define TEST_ASSERT_EQUAL_HEX16_MESSAGE(expected, actual, msg) \
    UnityAssertEqualHex((unsigned int)(uint16_t)(expected), (unsigned int)(uint16_t)(actual), (msg), __LINE__)
#define TEST_ASSERT_EQUAL_HEX32_MESSAGE(expected, actual, msg) \
    UnityAssertEqualHex((unsigned int)(uint32_t)(expected), (unsigned int)(uint32_t)(actual), (msg), __LINE__)

#define TEST_ASSERT_EQUAL_CHAR(expected, actual) \
    UnityAssertEqualInt((int)(char)(expected), (int)(char)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_CHAR_MESSAGE(expected, actual, msg) \
    UnityAssertEqualInt((int)(char)(expected), (int)(char)(actual), (msg), __LINE__)

/* ==========================================
 * String assertions
 * ========================================== */
#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    UnityAssertEqualString((const char *)(expected), (const char *)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_STRING_MESSAGE(expected, actual, msg) \
    UnityAssertEqualString((const char *)(expected), (const char *)(actual), (msg), __LINE__)
#define TEST_ASSERT_NOT_EQUAL_STRING(expected, actual) \
    UnityAssertNotEqualString((const char *)(expected), (const char *)(actual), NULL, __LINE__)
#define TEST_ASSERT_NOT_EQUAL_STRING_MESSAGE(expected, actual, msg) \
    UnityAssertNotEqualString((const char *)(expected), (const char *)(actual), (msg), __LINE__)

/* ==========================================
 * Pointer assertions
 * ========================================== */
#define TEST_ASSERT_EQUAL_PTR(expected, actual) \
    UnityAssertEqualPointer((const void *)(expected), (const void *)(actual), NULL, __LINE__)
#define TEST_ASSERT_EQUAL_PTR_MESSAGE(expected, actual, msg) \
    UnityAssertEqualPointer((const void *)(expected), (const void *)(actual), (msg), __LINE__)
#define TEST_ASSERT_NOT_EQUAL_PTR(expected, actual) \
    UnityAssertNotEqualPointer((const void *)(expected), (const void *)(actual), NULL, __LINE__)
#define TEST_ASSERT_NOT_EQUAL_PTR_MESSAGE(expected, actual, msg) \
    UnityAssertNotEqualPointer((const void *)(expected), (const void *)(actual), (msg), __LINE__)

/* ==========================================
 * Float assertions
 * ========================================== */
#define TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual) \
    UnityAssertFloatWithin((float)(delta), (float)(expected), (float)(actual), NULL, __LINE__)
#define TEST_ASSERT_FLOAT_WITHIN_MESSAGE(delta, expected, actual, msg) \
    UnityAssertFloatWithin((float)(delta), (float)(expected), (float)(actual), (msg), __LINE__)
#define TEST_ASSERT_FLOAT_EQUAL(expected, actual) \
    UnityAssertFloatWithin(UNITY_FLOAT_PRECISION, (float)(expected), (float)(actual), NULL, __LINE__)
#define TEST_ASSERT_FLOAT_EQUAL_MESSAGE(expected, actual, msg) \
    UnityAssertFloatWithin(UNITY_FLOAT_PRECISION, (float)(expected), (float)(actual), (msg), __LINE__)
#define TEST_ASSERT_FLOAT_ARRAY_WITHIN(delta, expected, actual, num) \
    UnityAssertEqualFloatArray((const float *)(expected), (const float *)(actual), (num), (delta), NULL, __LINE__)
#define TEST_ASSERT_FLOAT_ARRAY_WITHIN_MESSAGE(delta, expected, actual, num, msg) \
    UnityAssertEqualFloatArray((const float *)(expected), (const float *)(actual), (num), (delta), (msg), __LINE__)

/* ==========================================
 * Double assertions
 * ========================================== */
#define TEST_ASSERT_DOUBLE_WITHIN(delta, expected, actual) \
    UnityAssertDoubleWithin((double)(delta), (double)(expected), (double)(actual), NULL, __LINE__)
#define TEST_ASSERT_DOUBLE_WITHIN_MESSAGE(delta, expected, actual, msg) \
    UnityAssertDoubleWithin((double)(delta), (double)(expected), (double)(actual), (msg), __LINE__)
#define TEST_ASSERT_DOUBLE_ARRAY_WITHIN(delta, expected, actual, num) \
    UnityAssertEqualDoubleArray((const double *)(expected), (const double *)(actual), (num), (delta), NULL, __LINE__)

/* ==========================================
 * Bit assertions
 * ========================================== */
#define TEST_ASSERT_BITS(mask, expected, actual) \
    UnityAssertEqualBits((unsigned int)(mask), (unsigned int)(expected), (unsigned int)(actual), NULL, __LINE__)
#define TEST_ASSERT_BITS_MESSAGE(mask, expected, actual, msg) \
    UnityAssertEqualBits((unsigned int)(mask), (unsigned int)(expected), (unsigned int)(actual), (msg), __LINE__)
#define TEST_ASSERT_BIT_HIGH(bit, actual) \
    UnityAssertEqualBits((unsigned int)(1U << (bit)), 1U, (unsigned int)(actual), NULL, __LINE__)
#define TEST_ASSERT_BIT_LOW(bit, actual) \
    UnityAssertEqualBits((unsigned int)(1U << (bit)), 0U, (unsigned int)(actual), NULL, __LINE__)
#define TEST_ASSERT_HEX8_WITHIN(delta, expected, actual) \
    UnityAssertNumbersWithin((const void *)(unsigned int)(uint8_t)(expected), \
                            (const void *)(unsigned int)(uint8_t)(actual), \
                            sizeof(uint8_t), NULL, __LINE__)
#define TEST_ASSERT_HEX16_WITHIN(delta, expected, actual) \
    UnityAssertNumbersWithin((const void *)(unsigned int)(uint16_t)(expected), \
                            (const void *)(unsigned int)(uint16_t)(actual), \
                            sizeof(uint16_t), NULL, __LINE__)
#define TEST_ASSERT_HEX32_WITHIN(delta, expected, actual) \
    UnityAssertNumbersWithin((const void *)(unsigned int)(uint32_t)(expected), \
                            (const void *)(unsigned int)(uint32_t)(actual), \
                            sizeof(uint32_t), NULL, __LINE__)

/* ==========================================
 * Test group macros
 * ========================================== */
#define TEST_GROUP(name) \
    extern const char *Unity_TestGroup; \
    static void setUp(void); \
    static void tearDown(void)

#define TEST_SET_UP(group)    void setUp_##group(void)
#define TEST_TEAR_DOWN(group) void tearDown_##group(void)

#define TEST_GROUP_RUNNER(group) void TEST_GROUP_RUNNER_##group(void)
#define TEST_GROUP_RUNNER_DECLARE(group) extern void TEST_GROUP_RUNNER_##group(void)
#define RUN_TEST_CASE(group, name) TEST_##group##_##name()
#define RUN_TEST_GROUP(group) TEST_GROUP_RUNNER_##group()

/* Test function macro - expands to a function declaration + opening brace.
   Test body follows on subsequent lines, closed with a bare `}`.
   Example:
       TEST(MyGroup, MyTest) {
           TEST_ASSERT_TRUE(x > 0);
       }
 */
#define TEST(group, name) \
    void TEST_##group##_##name(void) {

/* ==========================================
 * Internal assertion functions (called by macros)
 * ========================================== */
void UnityAssertEqualInt(int expected, int actual, const char *msg, unsigned int line);
void UnityAssertNotEqualInt(int expected, int actual, const char *msg, unsigned int line);
void UnityAssertEqualUnsigned(unsigned int expected, unsigned int actual, const char *msg, unsigned int line);
void UnityAssertEqualHex(unsigned int expected, unsigned int actual, const char *msg, unsigned int line);
void UnityAssertEqualString(const char *expected, const char *actual, const char *msg, unsigned int line);
void UnityAssertNotEqualString(const char *expected, const char *actual, const char *msg, unsigned int line);
void UnityAssertEqualPointer(const void *expected, const void *actual, const char *msg, unsigned int line);
void UnityAssertNotEqualPointer(const void *expected, const void *actual, const char *msg, unsigned int line);
void UnityAssertEqualBits(unsigned int mask, unsigned int expected, unsigned int actual, const char *msg, unsigned int line);
void UnityAssertEqualFloatArray(const float *expected, const float *actual,
                                unsigned int num_elements, float tolerance,
                                const char *msg, unsigned int line);
void UnityAssertFloatWithin(float delta, float expected, float actual,
                            const char *msg, unsigned int line);
void UnityAssertEqualDoubleArray(const double *expected, const double *actual,
                                 unsigned int num_elements, double tolerance,
                                 const char *msg, unsigned int line);
void UnityAssertDoubleWithin(double delta, double expected, double actual,
                             const char *msg, unsigned int line);
void UnityAssertNumbersWithin(const void *expected, const void *actual,
                              unsigned int num_bytes, const char *msg,
                              unsigned int line);

/* ==========================================
 * Test runner utilities
 * ========================================== */
typedef void (*UnityTestFunction)(void);
void UnityDefaultTestRun(UnityTestFunction func, const char *funcName, unsigned int line);
unsigned int UnityNumberOfCases(void);

/* ==========================================
 * Print utilities
 * ========================================== */
void UnityPrint(const char *msg);
void UnityPrintNumber(int number);
void UnityPrintNumberUnsigned(unsigned int number);
void UnityPrintNumberHex(unsigned int number, unsigned int nibbles);
const char *UnityGetColor(unsigned int result);
void UnityPrintSummary(void);
unsigned int UnityGetFailureCount(void);
unsigned int UnityGetIgnoreCount(void);
void UnityConcludeTest(void);

#ifdef __cplusplus
}
#endif

#endif /* UNITY_H_ */
