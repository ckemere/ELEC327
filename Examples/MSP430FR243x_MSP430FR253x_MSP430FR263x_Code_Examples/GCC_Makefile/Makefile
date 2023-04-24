#===============================================================
#Makefile for building MSP Code Examples in command line
#environement using the GCC Open Source Compiler for MSP430
#===============================================================

# Require DEVICE to be specified
ifndef DEVICE
$(info Please specify a device, e.g. DEVICE=MSP430F5529)
$(error unspecified device)
endif
# Require EXAMPLE to be specified
ifndef EXAMPLE
$(info Please specify an example, e.g. EXAMPLE=MSP430F55xx_adc_01)
$(error unspecified example)
endif

###################### Windows OS ######################
ifeq ($(OS),Windows_NT)
	################## GCC Root Variable ###################
	GCC_DIR     ?= '$(HOMEDRIVE)$(HOMEPATH)/ti/gcc'
	GCC_MSP_INC_DIR ?= $(GCC_DIR)/include
	LDDIR       := $(GCC_MSP_INC_DIR)/$(DEVICE)
	RM          := rd /s /q
################### Linux or Mac OS ####################
else
	################## GCC Root Variable ###################
	GCC_DIR     ?= ~/ti/gcc
	GCC_MSP_INC_DIR ?= $(GCC_DIR)/include
	LDDIR       := $(GCC_MSP_INC_DIR)/$(shell echo $(DEVICE) | tr A-Z a-z)
	RM          := rm -rf
endif

######################################
GCC_BIN_DIR     ?= $(GCC_DIR)/bin
GCC_INC_DIR     ?= $(GCC_DIR)/msp430-elf/include
######################################
CC              := $(GCC_BIN_DIR)/msp430-elf-gcc
GDB			    := $(GCC_BIN_DIR)/msp430-elf-gdb
######################################
CFLAGS          := -Os -D__$(DEVICE)__ -mmcu=$(DEVICE) -g -ffunction-sections -fdata-sections -DDEPRECATED
LDFLAGS         := -T $(LDDIR).ld -L $(GCC_MSP_INC_DIR) -mmcu=$(DEVICE) -g -Wl,--gc-sections
INCLUDES        := -I $(GCC_MSP_INC_DIR) -I $(GCC_INC_DIR)
######################################
SRC             := ../C/$(EXAMPLE).c
EXOBJECT        := $(EXAMPLE)/$(EXAMPLE).o
EXOUTPUT        := $(EXAMPLE)/$(EXAMPLE).out
######################################

all: $(EXOUTPUT)

$(EXAMPLE):
	@mkdir $(EXAMPLE)

$(EXOBJECT): ${SRC}
	@echo ============================================
	@echo Compiling $(EXAMPLE).c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

$(EXOUTPUT): $(EXAMPLE) $(EXOBJECT)
	@echo ============================================
	@echo Linking objects and generating output binary
	$(CC) $(LDFLAGS) $(EXOBJECT) -o $@

debug: all
	$(GDB) $(EXAMPLE)/$(EXAMPLE).out

clean:
	@$(RM) $(EXAMPLE)