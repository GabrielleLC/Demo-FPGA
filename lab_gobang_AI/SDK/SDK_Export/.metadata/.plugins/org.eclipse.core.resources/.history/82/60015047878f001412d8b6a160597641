################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/AI.c \
../src/VGADraw.c \
../src/lab_gobang_AI_2.c 

LD_SRCS += \
../src/lscript.ld 

OBJS += \
./src/AI.o \
./src/VGADraw.o \
./src/lab_gobang_AI_2.o 

C_DEPS += \
./src/AI.d \
./src/VGADraw.d \
./src/lab_gobang_AI_2.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo Building file: $<
	@echo Invoking: MicroBlaze gcc compiler
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -I../../standalone_bsp_0/microblaze_0/include -mxl-barrel-shift -mxl-pattern-compare -mcpu=v8.20.b -mno-xl-soft-mul -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo Finished building: $<
	@echo ' '


