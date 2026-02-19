################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
../User_Architect/Src/user_codec.c \
../User_Architect/Src/user_delay.c \
../User_Architect/Src/user_queue.c \
../User_Architect/Src/user_ring_buffe.c 

OBJS += \
./User_Architect/Src/user_codec.o \
./User_Architect/Src/user_delay.o \
./User_Architect/Src/user_queue.o \
./User_Architect/Src/user_ring_buffe.o 

C_DEPS += \
./User_Architect/Src/user_codec.d \
./User_Architect/Src/user_delay.d \
./User_Architect/Src/user_queue.d \
./User_Architect/Src/user_ring_buffe.d 


# 每个子目录必须为构建它所贡献的源提供规则
User_Architect/Src/%.o User_Architect/Src/%.su User_Architect/Src/%.cyclo: ../User_Architect/Src/%.c User_Architect/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../User_Application/Inc -I../User_Algorithm/Inc -I../User_Architect/Inc -I../User_Drives/Inc -I../User_Lib/Inc -I../User_Status/Inc -I../Middlewares/ST/ARM/DSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User_Architect-2f-Src

clean-User_Architect-2f-Src:
	-$(RM) ./User_Architect/Src/user_codec.cyclo ./User_Architect/Src/user_codec.d ./User_Architect/Src/user_codec.o ./User_Architect/Src/user_codec.su ./User_Architect/Src/user_delay.cyclo ./User_Architect/Src/user_delay.d ./User_Architect/Src/user_delay.o ./User_Architect/Src/user_delay.su ./User_Architect/Src/user_queue.cyclo ./User_Architect/Src/user_queue.d ./User_Architect/Src/user_queue.o ./User_Architect/Src/user_queue.su ./User_Architect/Src/user_ring_buffe.cyclo ./User_Architect/Src/user_ring_buffe.d ./User_Architect/Src/user_ring_buffe.o ./User_Architect/Src/user_ring_buffe.su

.PHONY: clean-User_Architect-2f-Src

