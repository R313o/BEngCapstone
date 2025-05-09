################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/_MULTI_FX_MEM/mem_manager_multi_fx.c 

OBJS += \
./Core/Src/_MULTI_FX_MEM/mem_manager_multi_fx.o 

C_DEPS += \
./Core/Src/_MULTI_FX_MEM/mem_manager_multi_fx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/_MULTI_FX_MEM/%.o Core/Src/_MULTI_FX_MEM/%.su Core/Src/_MULTI_FX_MEM/%.cyclo: ../Core/Src/_MULTI_FX_MEM/%.c Core/Src/_MULTI_FX_MEM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-_MULTI_FX_MEM

clean-Core-2f-Src-2f-_MULTI_FX_MEM:
	-$(RM) ./Core/Src/_MULTI_FX_MEM/mem_manager_multi_fx.cyclo ./Core/Src/_MULTI_FX_MEM/mem_manager_multi_fx.d ./Core/Src/_MULTI_FX_MEM/mem_manager_multi_fx.o ./Core/Src/_MULTI_FX_MEM/mem_manager_multi_fx.su

.PHONY: clean-Core-2f-Src-2f-_MULTI_FX_MEM

