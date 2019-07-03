################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../alt_timers.c \
../main.c \
../parser.c \
../testsdram.c \
../transfer_data.c 

OBJS += \
./alt_timers.o \
./main.o \
./parser.o \
./testsdram.o \
./transfer_data.o 

C_DEPS += \
./alt_timers.d \
./main.d \
./parser.d \
./testsdram.d \
./transfer_data.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler 4 [arm-linux-gnueabihf]'
	arm-linux-gnueabihf-gcc -O0 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


