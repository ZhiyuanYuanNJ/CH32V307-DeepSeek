################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/SRC/Startup/startup_ch32v30x_D8C.S 

S_UPPER_DEPS += \
./Startup/startup_ch32v30x_D8C.d 

OBJS += \
./Startup/startup_ch32v30x_D8C.o 



# Each subdirectory must supply rules for building sources it contributes
Startup/startup_ch32v30x_D8C.o: c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/SRC/Startup/startup_ch32v30x_D8C.S
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -x assembler-with-cpp -I"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/SRC/Startup" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

