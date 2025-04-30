################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/chorus.c \
../Core/Src/effect.c \
../Core/Src/emt_140_dark_3.c \
../Core/Src/h1_gaincorrected.c \
../Core/Src/h2_gaincorrected.c \
../Core/Src/h3_gaincorrected.c \
../Core/Src/main.c \
../Core/Src/partitioned_fir_convolution_fft.c \
../Core/Src/pipe.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/supro_simulation.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c 

OBJS += \
./Core/Src/chorus.o \
./Core/Src/effect.o \
./Core/Src/emt_140_dark_3.o \
./Core/Src/h1_gaincorrected.o \
./Core/Src/h2_gaincorrected.o \
./Core/Src/h3_gaincorrected.o \
./Core/Src/main.o \
./Core/Src/partitioned_fir_convolution_fft.o \
./Core/Src/pipe.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/supro_simulation.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o 

C_DEPS += \
./Core/Src/chorus.d \
./Core/Src/effect.d \
./Core/Src/emt_140_dark_3.d \
./Core/Src/h1_gaincorrected.d \
./Core/Src/h2_gaincorrected.d \
./Core/Src/h3_gaincorrected.d \
./Core/Src/main.d \
./Core/Src/partitioned_fir_convolution_fft.d \
./Core/Src/pipe.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/supro_simulation.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H743xx -DUSE_PWR_LDO_SUPPLY -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -Ofast -ffunction-sections -fdata-sections -Wall -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -ffast-math -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/chorus.cyclo ./Core/Src/chorus.d ./Core/Src/chorus.o ./Core/Src/chorus.su ./Core/Src/effect.cyclo ./Core/Src/effect.d ./Core/Src/effect.o ./Core/Src/effect.su ./Core/Src/emt_140_dark_3.cyclo ./Core/Src/emt_140_dark_3.d ./Core/Src/emt_140_dark_3.o ./Core/Src/emt_140_dark_3.su ./Core/Src/h1_gaincorrected.cyclo ./Core/Src/h1_gaincorrected.d ./Core/Src/h1_gaincorrected.o ./Core/Src/h1_gaincorrected.su ./Core/Src/h2_gaincorrected.cyclo ./Core/Src/h2_gaincorrected.d ./Core/Src/h2_gaincorrected.o ./Core/Src/h2_gaincorrected.su ./Core/Src/h3_gaincorrected.cyclo ./Core/Src/h3_gaincorrected.d ./Core/Src/h3_gaincorrected.o ./Core/Src/h3_gaincorrected.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/partitioned_fir_convolution_fft.cyclo ./Core/Src/partitioned_fir_convolution_fft.d ./Core/Src/partitioned_fir_convolution_fft.o ./Core/Src/partitioned_fir_convolution_fft.su ./Core/Src/pipe.cyclo ./Core/Src/pipe.d ./Core/Src/pipe.o ./Core/Src/pipe.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/supro_simulation.cyclo ./Core/Src/supro_simulation.d ./Core/Src/supro_simulation.o ./Core/Src/supro_simulation.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h7xx.cyclo ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/system_stm32h7xx.su

.PHONY: clean-Core-2f-Src

