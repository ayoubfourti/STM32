################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Modules/Display/liquidcrystal_i2c.c 

C_DEPS += \
./Modules/Display/liquidcrystal_i2c.d 

OBJS += \
./Modules/Display/liquidcrystal_i2c.o 


# Each subdirectory must supply rules for building sources it contributes
Modules/Display/liquidcrystal_i2c.o: ../Modules/Display/liquidcrystal_i2c.c Modules/Display/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Modules/Actuators -I../Modules/AI -I../Modules/Sensors -I../Modules/Events -I../Modules/Display -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/admin/Desktop/calculateur/PROJECT/CODE/STM32Project/Modules" -I../Middlewares/ST/AI/Inc -I../X-CUBE-AI/App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Modules/Display/liquidcrystal_i2c.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

