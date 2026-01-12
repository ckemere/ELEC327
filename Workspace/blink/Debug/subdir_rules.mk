################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/bin/tiarmclang" -c -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"/Users/ckemere/Code/elec327-workspace/Blink" -I"/Users/ckemere/ti/mspm0_sdk_2_03_00_07/source" -I"/Users/ckemere/ti/mspm0_sdk_2_03_00_07/source/third_party/CMSIS/Core/Include" -I"/Applications/ti/ccs2040/ccs/tools/compiler/ti-cgt-armllvm_4.0.4.LTS/include" -D__MSPM0G3507__ -gdwarf-3 -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


