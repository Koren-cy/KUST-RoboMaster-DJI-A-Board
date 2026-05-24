# Test Framework

A host-side unit test framework for the RoboMaster DJI A-Board firmware project.

## 架构设计

```
Test_Framework/
├── CMakeLists.txt          # Build configuration
├── README.md
├── unity/
│   ├── unity.c             # Unity test framework (mock files, no modification)
│   └── unity.h
├── mocks/
│   ├── mock_main.h         # Stubs for Core/Inc/main.h
│   ├── mock_stm32f4xx_hal.h # Stubs for HAL drivers
│   └── mock_SEGGER_RTT.h   # Stubs for SEGGER RTT
├── runner/
│   └── test_runner.c       # Main entry, runs all TEST_GROUP calls
└── tests/
    ├── test_newton.c       # Tests for user_newton module
    └── test_sgdm.c         # Tests for user_sgdm module
```

## 构建

```bash
# Windows (MSVC)
mkdir build && cd build
cmake -G "NMake Makefiles" ..
cmake --build .

# Windows (MinGW)
mkdir build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .

# Linux / macOS
mkdir build && cd build
cmake ..
cmake --build .
```

## 运行测试

```bash
# 直接运行单个测试
./bin/test_newton.exe   # Windows: bin\test_newton.exe

# 或使用 CTest
ctest --output-on-failure
```

## 添加新测试

1. 在 `tests/` 目录下创建 `test_<module>.c`
2. 包含 Unity 头文件和被测模块的头文件
3. 使用以下宏编写测试:

```c
#include "unity.h"
#include "unity_fixture.h"   // 包含 TEST_GROUP 宏

TEST_GROUP(MyModule);

TEST_SETUP(MyModule) {
    // 每个测试前运行
}

TEST_TEAR_DOWN(MyModule) {
    // 每个测试后运行
}

TEST(MyModule, TestName) {
    TEST_ASSERT_EQUAL_INT(1, some_function());
    TEST_ASSERT_FLOAT_WITHIN(epsilon, expected, actual);
}

TEST_GROUP_RUNNER(MyModule) {
    RUN_TEST_CASE(MyModule, TestName);
}

extern TEST_GROUP_RUNNER(OtherGroup);  // 来自其他文件的 TEST_GROUP

ALL_TEST_GROUPS_RUNNER {
    RUN_TEST_GROUP(MyModule);
    RUN_TEST_GROUP(OtherGroup);
}
```

4. 在 `runner/test_runner.c` 的 `ALL_TEST_GROUPS_RUNNER` 中添加
   `RUN_TEST_GROUP(MyModule);`
