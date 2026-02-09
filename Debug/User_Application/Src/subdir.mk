################################################################################
# 自动生成的文件。不要编辑！
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# 将这些工具调用的输入和输出添加到构建变量 
C_SRCS += \
../User_Application/Src/swerve_chassis.c 

OBJS += \
./User_Application/Src/swerve_chassis.o 

C_DEPS += \
./User_Application/Src/swerve_chassis.d 


# 每个子目录必须为构建它所贡献的源提供规则
User_Application/Src/%.o User_Application/Src/%.su User_Application/Src/%.cyclo: ../User_Application/Src/%.c User_Application/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F427xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../User_Application/Inc -I../User_Algorithm/Inc -I../User_Architect/Inc -I../User_Drives/Inc -I../User_Lib/Inc -I../User_Status/Inc -I../Middlewares/ST/ARM/DSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User_Application-2f-Src

clean-User_Application-2f-Src:
	-$(RM) ./User_Application/Src/swerve_chassis.cyclo ./User_Application/Src/swerve_chassis.d ./User_Application/Src/swerve_chassis.o ./User_Application/Src/swerve_chassis.su

.PHONY: clean-User_Application-2f-Src

