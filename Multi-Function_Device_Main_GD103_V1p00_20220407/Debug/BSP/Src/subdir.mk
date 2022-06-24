################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../BSP/Src/BT_Comm.c \
../BSP/Src/CapTouch_I2C.c \
../BSP/Src/DEV_Config.c \
../BSP/Src/GUI_Paint.c \
../BSP/Src/IOExpander_I2C.c \
../BSP/Src/KeyScanTask.c \
../BSP/Src/LCD_Driver.c \
../BSP/Src/SystemProcess.c \
../BSP/Src/Voice_I2C.c \
../BSP/Src/a_image2_0429.c \
../BSP/Src/factory_image.c \
../BSP/Src/user_func.c 

OBJS += \
./BSP/Src/BT_Comm.o \
./BSP/Src/CapTouch_I2C.o \
./BSP/Src/DEV_Config.o \
./BSP/Src/GUI_Paint.o \
./BSP/Src/IOExpander_I2C.o \
./BSP/Src/KeyScanTask.o \
./BSP/Src/LCD_Driver.o \
./BSP/Src/SystemProcess.o \
./BSP/Src/Voice_I2C.o \
./BSP/Src/a_image2_0429.o \
./BSP/Src/factory_image.o \
./BSP/Src/user_func.o 

C_DEPS += \
./BSP/Src/BT_Comm.d \
./BSP/Src/CapTouch_I2C.d \
./BSP/Src/DEV_Config.d \
./BSP/Src/GUI_Paint.d \
./BSP/Src/IOExpander_I2C.d \
./BSP/Src/KeyScanTask.d \
./BSP/Src/LCD_Driver.d \
./BSP/Src/SystemProcess.d \
./BSP/Src/Voice_I2C.d \
./BSP/Src/a_image2_0429.d \
./BSP/Src/factory_image.d \
./BSP/Src/user_func.d 


# Each subdirectory must supply rules for building sources it contributes
BSP/Src/%.o: ../BSP/Src/%.c BSP/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../BSP/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

