################################################################################
# Makefile for MSPM0+ using Ti clang
################################################################################



################################################################################
# Directories which will be system specific
TI_ROOT := /Applications/ti/ccs1220/ccs
SDK_ROOT := /Users/ckemere/ti/mspm0_sdk_2_03_00_07/source


################################################################################
# Relative paths to various other important directories
CLANG_ROOT := $(TI_ROOT)/tools/compiler/ti-cgt-armllvm_3.2.1.LTS
CMSIS_ROOT := $(SDK_ROOT)/third_party/CMSIS/Core

# Commands
PROGRAMING_CMD := $(TI_ROOT)/ccs_base/DebugServer/bin/DSLite
CC := $(TI_ROOT)/tools/compiler/ti-cgt-armllvm_3.2.1.LTS/bin/tiarmclang

################################################################################
# Details specific to the MSPM03507.
DEVICE_FLAG := -D__MSPM0G3507__ 
# These files were copied from CCS compiles. I assume they are built by "sysconfig_cli.sh"
DEVICE_CONFIG_FILE := ../MSPM0G3507.ccxml
LINKER_MEMORY_TABLE__FLAG := -Wl,-l"../device_linker.cmd" 


################################################################################
# Now the actual Makefile stuff

# LOCAL_OBJS should be a list of object files for each C file that is part of the project
LOCAL_OBJS := gpio_toggle_output.o
SYSTEM_OBJS := startup_mspm0g350x_ticlang.o
BINARY_NAME := gpio_toggle_output_LP_MSPM0G3507_nortos_ticlang

SYSTEM_INCLUDES := -I"$(SDK_ROOT)" -I"$(CMSIS_ROOT)/Include"

RM := rm -rf
RMDIR := rm -rf

LIBS := -Wl,-llibc.a

# We're autodetecting header dependencies, so let's make sure that we check for changes
-include $(LOCAL_OBJS:.o=.d_raw)

%.o: %.c
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	$(CC) -c -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 \
			-I. $(SYSTEM_INCLUDES) -gdwarf-3 -MMD -MP \
			-MF"$(basename $(<F)).d_raw" -MT"$(@)" $(DEVICE_FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

startup_mspm0g350x_ticlang.o: $(SDK_ROOT)/ti/devices/msp/m0p/startup_system_files/ticlang/startup_mspm0g350x_ticlang.c
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	$(CC) -c	-march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 \
	-I.	$(SYSTEM_INCLUDES) -gdwarf-3 -MMD -MP \
	-MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(DEVICE_FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


# All Target
all: $(LOCAL_OBJS) $(SYSTEM_OBJS)
	@$(MAKE) $(BINARY_NAME).out

# Tool invocations
$(BINARY_NAME).out: $(LOCAL_OBJS) $(SYSTEM_OBJS)
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	$(CC) \
	      $(DEVICE_FLAG)   \
				-march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -gdwarf-3 \
				-Wl,-m"$(BINARY_NAME).map" \
				-Wl,-i"$(SDK_ROOT)" \
	      -Wl,-i"$(CLANG_ROOT)/lib" \
				-Wl,--diag_wrap=off -Wl,--display_error_number -Wl,--warn_sections \
				-Wl,--xml_link_info="$(BINARY_NAME)_linkInfo.xml" \
				-Wl,--rom_model \
				$(LOCAL_OBJS) $(SYSTEM_OBJS)  \
				$(LINKER_MEMORY_TABLE__FLAG) \
				$(LIBS) \
				-o $(BINARY_NAME).out

	@echo 'Finished building target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LOCAL_OBJS) $(SYSTEM_OBJS)
	-$(RM) *.d_raw
	-$(RM) $(BINARY_NAME)*.*
	-@echo 'Finished clean'
	-@echo ' '

program:
	-$(PROGRAMING_CMD) flash --config=$(DEVICE_CONFIG_FILE) $(BINARY_NAME).out -e -f -v 

.PHONY: all clean program dependents
.SECONDARY:

