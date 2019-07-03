################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../alt_clock_manager.c \
../alt_globaltmr.c \
../alt_timers.c \
../alt_watchdog.c \
../main.c \
../parser.c \
../testsdram.c \
../transfer_data.c 

OBJS += \
./alt_clock_manager.o \
./alt_globaltmr.o \
./alt_timers.o \
./alt_watchdog.o \
./main.o \
./parser.o \
./testsdram.o \
./transfer_data.o 

C_DEPS += \
./alt_clock_manager.d \
./alt_globaltmr.d \
./alt_timers.d \
./alt_watchdog.d \
./main.d \
./parser.d \
./testsdram.d \
./transfer_data.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 4 [arm-linux-gnueabihf]'
	arm-linux-gnueabihf-gcc -Dsoc_cv_av -I/home/snoeijs/intelFPGA/18.1/embedded/ip/altera/hps/altera_hps/hwlib/include -I/home/snoeijs/intelFPGA/18.1/embedded/ip/altera/hps/altera_hps/hwlib/include/soc_cv_av -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


