################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/ETH/NetLib/eth_driver_10M.c 

C_DEPS += \
./NetLib/eth_driver_10M.d 

OBJS += \
./NetLib/eth_driver_10M.o 



# Each subdirectory must supply rules for building sources it contributes
NetLib/eth_driver_10M.o: c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/ETH/NetLib/eth_driver_10M.c
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -I"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/ETH/NetLib" -I"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/SRC/Core" -I"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/SRC/Debug" -I"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/SRC/Peripheral/inc" -I"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/ETH/TCPClient/User" -I"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/ETH/TCPClient/mbedtls/include" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

