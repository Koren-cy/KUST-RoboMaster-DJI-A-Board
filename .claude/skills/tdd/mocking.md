# When to Mock

Mock at **system boundaries** only:

- External APIs / hardware peripherals
- Time/randomness
- File system (sometimes)

Don't mock:

- Your own modules
- Internal collaborators
- Anything you control

## Designing for Mockability in Embedded C

### 1. Use dependency injection

Pass external dependencies in rather than creating them internally:

```c
// Easy to test — dependencies injected
float PID_Calculate(PID_Controller *pid, float setpoint, float measurement);

// Hard to test — dependencies hidden inside
void Motor_Init(void);  // internally calls HAL_TIM_Init, needs hardware
```

### 2. Separate pure logic from hardware interaction

```c
// Extract pure logic — testable on host
float swerve_kinematics_calc_wheel_angle(float vx, float vy, float omega);

// Hardware wrapper — test on target only
void swerve_set_wheel_angle(uint8_t wheel, float angle);
```

### 3. Mock HAL at the header level

For host testing, create a mock header that replaces CMSIS register definitions with plain RAM variables, then compile tests with the host GCC instead of arm-none-eabi-gcc.
