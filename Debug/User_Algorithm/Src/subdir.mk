################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
../User_Algorithm/Src/user_coord.c \
../User_Algorithm/Src/user_pid.c 

OBJS += \
./User_Algorithm/Src/user_coord.o \
./User_Algorithm/Src/user_pid.o 

C_DEPS += \
./User_Algorithm/Src/user_coord.d \
./User_Algorithm/Src/user_pid.d 


# 每个子目录必须为构建它所贡献的源提供规则
User_Algorithm/Src/%.o User_Algorithm/Src/%.su User_Algorithm/Src/%.cyclo: ../User_Algorithm/Src/%.c User_Algorithm/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../User_Application/Inc -I../User_Algorithm/Inc -I../User_Architect/Inc -I../User_Drives/Inc -I../User_Lib/Inc -I../User_Status/Inc -I../Middlewares/ST/ARM/DSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User_Algorithm-2f-Src

clean-User_Algorithm-2f-Src:
	-$(RM) ./User_Algorithm/Src/user_coord.cyclo ./User_Algorithm/Src/user_coord.d ./User_Algorithm/Src/user_coord.o ./User_Algorithm/Src/user_coord.su ./User_Algorithm/Src/user_pid.cyclo ./User_Algorithm/Src/user_pid.d ./User_Algorithm/Src/user_pid.o ./User_Algorithm/Src/user_pid.su

.PHONY: clean-User_Algorithm-2f-Src

