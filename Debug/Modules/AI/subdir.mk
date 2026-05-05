################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Modules/AI/ai_monitor.c 

CPP_SRCS += \
../Modules/AI/ai_aquarium.cpp 

C_DEPS += \
./Modules/AI/ai_monitor.d 

OBJS += \
./Modules/AI/ai_aquarium.o \
./Modules/AI/ai_monitor.o 

CPP_DEPS += \
./Modules/AI/ai_aquarium.d 


# Each subdirectory must supply rules for building sources it contributes
Modules/AI/ai_aquarium.o: ../Modules/AI/ai_aquarium.cpp Modules/AI/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m4 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Modules/Actuators -I../Modules/AI -I../Modules/Sensors -I../Modules/Events -I../Modules/Display -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/AI/Inc -I../X-CUBE-AI/App -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"Modules/AI/ai_aquarium.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Modules/AI/ai_monitor.o: ../Modules/AI/ai_monitor.c Modules/AI/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Modules/Actuators -I../Modules/AI -I../Modules/Sensors -I../Modules/Events -I../Modules/Display -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/admin/Desktop/calculateur/PROJECT/CODE/STM32Project/Modules" -I../Middlewares/ST/AI/Inc -I../X-CUBE-AI/App -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Modules/AI/ai_monitor.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

