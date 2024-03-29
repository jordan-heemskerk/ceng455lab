################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Generated_Code/AuxTask.c \
../Generated_Code/Cpu.c \
../Generated_Code/DdsTask.c \
../Generated_Code/Handler.c \
../Generated_Code/MainTask.c \
../Generated_Code/MonitorTask.c \
../Generated_Code/PeriodicGenerator.c \
../Generated_Code/SerialTask.c \
../Generated_Code/UserTask.c \
../Generated_Code/clockMan1.c \
../Generated_Code/fsl_hwtimer1.c \
../Generated_Code/fsl_mpu1.c \
../Generated_Code/hardware_init.c \
../Generated_Code/mqx_ksdk.c \
../Generated_Code/myUART.c \
../Generated_Code/osa1.c \
../Generated_Code/pin_init.c \
../Generated_Code/uart1.c 

OBJS += \
./Generated_Code/AuxTask.o \
./Generated_Code/Cpu.o \
./Generated_Code/DdsTask.o \
./Generated_Code/Handler.o \
./Generated_Code/MainTask.o \
./Generated_Code/MonitorTask.o \
./Generated_Code/PeriodicGenerator.o \
./Generated_Code/SerialTask.o \
./Generated_Code/UserTask.o \
./Generated_Code/clockMan1.o \
./Generated_Code/fsl_hwtimer1.o \
./Generated_Code/fsl_mpu1.o \
./Generated_Code/hardware_init.o \
./Generated_Code/mqx_ksdk.o \
./Generated_Code/myUART.o \
./Generated_Code/osa1.o \
./Generated_Code/pin_init.o \
./Generated_Code/uart1.o 

C_DEPS += \
./Generated_Code/AuxTask.d \
./Generated_Code/Cpu.d \
./Generated_Code/DdsTask.d \
./Generated_Code/Handler.d \
./Generated_Code/MainTask.d \
./Generated_Code/MonitorTask.d \
./Generated_Code/PeriodicGenerator.d \
./Generated_Code/SerialTask.d \
./Generated_Code/UserTask.d \
./Generated_Code/clockMan1.d \
./Generated_Code/fsl_hwtimer1.d \
./Generated_Code/fsl_mpu1.d \
./Generated_Code/hardware_init.d \
./Generated_Code/mqx_ksdk.d \
./Generated_Code/myUART.d \
./Generated_Code/osa1.d \
./Generated_Code/pin_init.d \
./Generated_Code/uart1.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK64FN1M0VLL12" -D"FSL_RTOS_MQX" -D"PEX_MQX_KSDK" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/hal/inc" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/hal/src/sim/MK64F12" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/system/src/clock/MK64F12" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/system/inc" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/osa/inc" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/CMSIS/Include" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/devices" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/devices/MK64F12/include" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/utilities/src" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/utilities/inc" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/devices/MK64F12/startup" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/Generated_Code/SDK/platform/devices/MK64F12/startup" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/Sources" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/Generated_Code" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/drivers/inc" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/psp/cortex_m" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/psp/cortex_m/cpu" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/config/common" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/include" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/bsp" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/psp/cortex_m/compiler/gcc_arm" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/nio" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/nio/src" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/nio/fs" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/nio/drivers/nio_dummy" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/nio/drivers/nio_serial" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx/source/nio/drivers/nio_tty" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx_stdlib/source/include" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/rtos/mqx/mqx_stdlib/source/stdio" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/hal/src/uart" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/SDK/platform/drivers/src/uart" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/Generated_Code/SDK/rtos/mqx/config/board" -I"C:/Users/jheems/git/ceng455lab3/deadline_scheduler/Generated_Code/SDK/rtos/mqx/config/mcu" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


