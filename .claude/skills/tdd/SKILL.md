---
name: tdd
description: Test-driven development. Use when the user wants to build features or fix bugs test-first, mentions "red-green-refactor", or wants tests.
---

# Test-Driven Development

TDD is the red → green loop. This skill is the reference that makes that loop produce tests worth keeping: what a good test is, where tests go, the anti-patterns, and the rules of the loop. Every section applies on every cycle — consult them before and during the loop, not after.

## What a good test is

Tests verify behavior through public interfaces, not implementation details. Code can change entirely; tests shouldn't. A good test reads like a specification — "PID controller clamps integral anti-windup" tells you exactly what capability exists — and survives refactors because it doesn't care about internal structure.

## Seams — where tests go

A **seam** is the public boundary you test at: the interface where you observe behavior without reaching inside. Tests live at seams, never against internals.

**Test only at pre-agreed seams.** Before writing any test, write down the seams under test and confirm them with the user. No test is written at an unconfirmed seam. You can't test everything — agreeing the seams up front is how testing effort lands on the critical paths and complex logic instead of every edge case.

Ask: "What's the public interface, and which seams should we test?"

## Anti-patterns

- **Implementation-coupled** — mocks internal collaborators, tests private (`static`) functions, or verifies through a side channel (reading a global variable instead of using the return value). The tell: the test breaks when you refactor but behavior hasn't changed.
- **Tautological** — the assertion recomputes the expected value the way the code does (`TEST_ASSERT_EQUAL_FLOAT(pid->Kp * error, proportional)`), so it passes by construction and can never disagree with the code. Expected values must come from an independent source of truth — a known-good literal, a worked example, the spec.
- **Horizontal slicing** — writing all tests first, then all implementation. Bulk tests verify _imagined_ behavior: you test the _shape_ of things rather than user-facing behavior, the tests go insensitive to real changes, and you commit to test structure before understanding the implementation. Work in **vertical slices** instead — one test → one implementation → repeat.

## Rules of the loop

- **Red before green.** Write the failing test first, then only enough code to pass it. Don't anticipate future tests or add speculative features.
- **One slice at a time.** One seam, one test, one minimal implementation per cycle.
- **Refactoring is not part of the loop.** It belongs to the review stage, not the red → green implementation cycle.

## Embedded C specifics

In bare-metal firmware, seams are typically:

| Seam Level | Example | Test Approach |
|-----------|---------|--------------|
| Algorithm module | `PID_Calculate()` | Host Unity test, no mocks needed |
| Data structure | `Queue_Push()`, `RingBuffer_Write()` | Host Unity test, no mocks |
| HAL wrapper | `Motor_SetSpeed()` | Host test with mocked HAL |
| ISR logic | `CAN_RxCpltCallback()` | Extract pure logic; test separately |
| Hardware driver | `PWM_Init()` | On-target test only (skip for unit testing) |

The key principle: **extract pure logic from hardware-coupled code.** If a 200-line function has 10 HAL calls, the 190 lines between them are still testable — extract them.
