################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include mbedtls/library/subdir.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include NetLib/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
TCPClient.hex \

SECONDARY_LIST += \
TCPClient.lst \

SECONDARY_SIZE += \
TCPClient.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: TCPClient.elf secondary-outputs

# Tool invocations
TCPClient.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -g -T "c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/ETH/TCPClient/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -L"c:/Users/OWNER/Downloads/CH32V307EVT/EVT/EXAM/ETH/NetLib" -Wl,-Map,"TCPClient.map" --specs=nano.specs --specs=nosys.specs -o "TCPClient.elf" $(OBJS) $(USER_OBJS) $(LIBS) -lprintf
TCPClient.hex: TCPClient.elf
	@	riscv-none-embed-objcopy -O ihex "TCPClient.elf" "TCPClient.hex"
TCPClient.lst: TCPClient.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle -M xw --line-numbers --wide "TCPClient.elf" > "TCPClient.lst"
TCPClient.siz: TCPClient.elf
	riscv-none-embed-size --format=berkeley "TCPClient.elf"

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) TCPClient.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets