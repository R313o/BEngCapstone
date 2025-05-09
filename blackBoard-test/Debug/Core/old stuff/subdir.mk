################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/old\ stuff/nodeSort.c \
../Core/old\ stuff/process.c \
../Core/old\ stuff/process.c.old.c 

OBJS += \
./Core/old\ stuff/nodeSort.o \
./Core/old\ stuff/process.o \
./Core/old\ stuff/process.c.old.o 

C_DEPS += \
./Core/old\ stuff/nodeSort.d \
./Core/old\ stuff/process.d \
./Core/old\ stuff/process.c.old.d 


# Each subdirectory must supply rules for building sources it contributes
Core/old\ stuff/nodeSort.o: ../Core/old\ stuff/nodeSort.c Core/old\ stuff/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/old stuff/nodeSort.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/old\ stuff/process.o: ../Core/old\ stuff/process.c Core/old\ stuff/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/old stuff/process.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/old\ stuff/process.c.old.o: ../Core/old\ stuff/process.c.old.c Core/old\ stuff/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Core/old stuff/process.c.old.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-old-20-stuff

clean-Core-2f-old-20-stuff:
	-$(RM) ./Core/old\ stuff/nodeSort.cyclo ./Core/old\ stuff/nodeSort.d ./Core/old\ stuff/nodeSort.o ./Core/old\ stuff/nodeSort.su ./Core/old\ stuff/process.c.old.cyclo ./Core/old\ stuff/process.c.old.d ./Core/old\ stuff/process.c.old.o ./Core/old\ stuff/process.c.old.su ./Core/old\ stuff/process.cyclo ./Core/old\ stuff/process.d ./Core/old\ stuff/process.o ./Core/old\ stuff/process.su

.PHONY: clean-Core-2f-old-20-stuff

