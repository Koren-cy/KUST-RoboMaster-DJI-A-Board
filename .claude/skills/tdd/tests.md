# Good and Bad Tests

## Good Tests

**Integration-style**: Test through real interfaces, not mocks of internal parts.

```c
// GOOD: Tests observable behavior through the public interface
void test_PID_controller_clamps_output(void) {
    PID_Controller pid;
    PID_Init(&pid, 2.0, 0.1, 0.05);
    PID_SetLimits(&pid, -100.0, 100.0);

    float output = PID_Calculate(&pid, 50.0, 0.0);  // huge error, will saturate

    TEST_ASSERT_FLOAT_WITHIN(0.01, 100.0, output);  // clamped to max
}
```

Characteristics:
- Tests behavior users/callers care about
- Uses public API only
- Survives internal refactors
- Describes WHAT, not HOW
- One logical assertion per test

## Bad Tests

**Implementation-detail tests**: Coupled to internal structure.

```c
// BAD: Tests internal struct fields instead of behavior
void test_PID_integral_field_updates(void) {
    PID_Controller pid;
    PID_Init(&pid, 2.0, 0.1, 0.05);

    PID_Calculate(&pid, 10.0, 0.0);

    TEST_ASSERT_FLOAT_WITHIN(0.01, 1.0, pid.integral);  // internal field!
}
```

Red flags:
- Testing private (`static`) functions or struct internals
- Asserting on implementation details
- Test breaks when refactoring without behavior change
- Test name describes HOW not WHAT

**Tautological tests**: Expected value restates the implementation.

```c
// BAD: Expected value is recomputed the way the code computes it
void test_PID_proportional_term(void) {
    PID_Controller pid;
    PID_Init(&pid, 2.0, 0.0, 0.0);

    float output = PID_Calculate(&pid, 10.0, 0.0);
    float expected = pid.Kp * (10.0 - 0.0);  // same calculation as the code!

    TEST_ASSERT_FLOAT_WITHIN(0.01, expected, output);
}

// GOOD: Expected value is an independent, known literal
void test_PID_proportional_term(void) {
    PID_Controller pid;
    PID_Init(&pid, 2.0, 0.0, 0.0);

    float output = PID_Calculate(&pid, 10.0, 0.0);

    TEST_ASSERT_FLOAT_WITHIN(0.01, 20.0, output);  // Kp=2.0 × error=10.0 = 20.0
}
```
