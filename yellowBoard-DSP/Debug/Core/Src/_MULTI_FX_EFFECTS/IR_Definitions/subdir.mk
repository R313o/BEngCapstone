################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/OD-M212-VINT-DYN-201-P05-00.c \
../Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/emt_140_dark_3.c \
../Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h1_gaincorrected.c \
../Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h2_gaincorrected.c \
../Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h3_gaincorrected.c 

OBJS += \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/OD-M212-VINT-DYN-201-P05-00.o \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/emt_140_dark_3.o \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h1_gaincorrected.o \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h2_gaincorrected.o \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h3_gaincorrected.o 

C_DEPS += \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/OD-M212-VINT-DYN-201-P05-00.d \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/emt_140_dark_3.d \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h1_gaincorrected.d \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h2_gaincorrected.d \
./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h3_gaincorrected.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/%.o Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/%.su Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/%.cyclo: ../Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/%.c Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H750xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-_MULTI_FX_EFFECTS-2f-IR_Definitions

clean-Core-2f-Src-2f-_MULTI_FX_EFFECTS-2f-IR_Definitions:
	-$(RM) ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/OD-M212-VINT-DYN-201-P05-00.cyclo ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/OD-M212-VINT-DYN-201-P05-00.d ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/OD-M212-VINT-DYN-201-P05-00.o ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/OD-M212-VINT-DYN-201-P05-00.su ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/emt_140_dark_3.cyclo ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/emt_140_dark_3.d ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/emt_140_dark_3.o ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/emt_140_dark_3.su ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h1_gaincorrected.cyclo ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h1_gaincorrected.d ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h1_gaincorrected.o ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h1_gaincorrected.su ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h2_gaincorrected.cyclo ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h2_gaincorrected.d ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h2_gaincorrected.o ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h2_gaincorrected.su ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h3_gaincorrected.cyclo ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h3_gaincorrected.d ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h3_gaincorrected.o ./Core/Src/_MULTI_FX_EFFECTS/IR_Definitions/h3_gaincorrected.su

.PHONY: clean-Core-2f-Src-2f-_MULTI_FX_EFFECTS-2f-IR_Definitions

