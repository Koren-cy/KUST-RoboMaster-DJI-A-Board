################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
../User_Drives/Src/user_can.c \
../User_Drives/Src/user_dji_bus.c \
../User_Drives/Src/user_dji_motor.c \
../User_Drives/Src/user_led.c \
../User_Drives/Src/user_pwm.c \
../User_Drives/Src/user_uart.c 

OBJS += \
./User_Drives/Src/user_can.o \
./User_Drives/Src/user_dji_bus.o \
./User_Drives/Src/user_dji_motor.o \
./User_Drives/Src/user_led.o \
./User_Drives/Src/user_pwm.o \
./User_Drives/Src/user_uart.o 

C_DEPS += \
./User_Drives/Src/user_can.d \
./User_Drives/Src/user_dji_bus.d \
./User_Drives/Src/user_dji_motor.d \
./User_Drives/Src/user_led.d \
./User_Drives/Src/user_pwm.d \
./User_Drives/Src/user_uart.d 


# 每个子目录必须为构建它所贡献的源提供规则
User_Drives/Src/%.o User_Drives/Src/%.su User_Drives/Src/%.cyclo: ../User_Drives/Src/%.c User_Drives/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../User_Application/Inc -I../User_Algorithm/Inc -I../User_Architect/Inc -I../User_Drives/Inc -I../User_Lib/Inc -I../User_Status/Inc -I../Middlewares/ST/ARM/DSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User_Drives-2f-Src

clean-User_Drives-2f-Src:
	-$(RM) ./User_Drives/Src/user_can.cyclo ./User_Drives/Src/user_can.d ./User_Drives/Src/user_can.o ./User_Drives/Src/user_can.su ./User_Drives/Src/user_dji_bus.cyclo ./User_Drives/Src/user_dji_bus.d ./User_Drives/Src/user_dji_bus.o ./User_Drives/Src/user_dji_bus.su ./User_Drives/Src/user_dji_motor.cyclo ./User_Drives/Src/user_dji_motor.d ./User_Drives/Src/user_dji_motor.o ./User_Drives/Src/user_dji_motor.su ./User_Drives/Src/user_led.cyclo ./User_Drives/Src/user_led.d ./User_Drives/Src/user_led.o ./User_Drives/Src/user_led.su ./User_Drives/Src/user_pwm.cyclo ./User_Drives/Src/user_pwm.d ./User_Drives/Src/user_pwm.o ./User_Drives/Src/user_pwm.su ./User_Drives/Src/user_uart.cyclo ./User_Drives/Src/user_uart.d ./User_Drives/Src/user_uart.o ./User_Drives/Src/user_uart.su

.PHONY: clean-User_Drives-2f-Src

