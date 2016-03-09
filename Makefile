PROJECT_NAME := $(shell basename $(CURDIR))

export OUTPUT_FILENAME
MAKEFILE_NAME := $(MAKEFILE_LIST)
MAKEFILE_DIR := $(dir $(MAKEFILE_NAME))
SDK_ROOT := $(shell echo $$SDK_ROOT)
PROJ_HOME := $(SDK_ROOT)/apps/$(PROJECT_NAME)

TEMPLATE_PATH = $(SDK_ROOT)/components/toolchain/gcc
ifeq ($(OS),Windows_NT)
include $(TEMPLATE_PATH)/Makefile.windows
else
include $(TEMPLATE_PATH)/Makefile.posix
endif

MK := mkdir
RM := rm -rf

#echo suspend
ifeq ("$(VERBOSE)","1")
NO_ECHO := 
else
NO_ECHO := @
endif

# Toolchain commands
CC              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc'
AS              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as'
AR              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar' -r
LD              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld'
NM              := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm'
OBJDUMP         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump'
OBJCOPY         := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy'
SIZE            := '$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size'

#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))


#Project specific files 
INC_PATHS  += -I$(abspath $(PROJ_HOME)/config)
INC_PATHS  += -I$(abspath $(PROJ_HOME)/include)
C_SOURCE_FILES += $(abspath $(PROJ_HOME)/$(wildcard *.c))


#source common to all targets
C_SOURCE_FILES += \
$(abspath $(SDK_ROOT)/components/libraries/button/app_button.c) \
$(abspath $(SDK_ROOT)/components/libraries/util/app_error.c) \
$(abspath $(SDK_ROOT)/components/libraries/timer/app_timer.c) \
$(abspath $(SDK_ROOT)/components/libraries/trace/app_trace.c) \
$(abspath $(SDK_ROOT)/components/libraries/util/nrf_assert.c) \
$(abspath $(SDK_ROOT)/components/libraries/uart/retarget.c) \
$(abspath $(SDK_ROOT)/components/libraries/uart/app_uart.c) \
$(abspath $(SDK_ROOT)/components/drivers_nrf/delay/nrf_delay.c) \
$(abspath $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c) \
$(abspath $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c) \
$(abspath $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c) \
$(abspath $(SDK_ROOT)/components/drivers_nrf/pstorage/pstorage.c) \
$(abspath $(SDK_ROOT)/examples/bsp/bsp.c) \
$(abspath $(SDK_ROOT)/examples/bsp/bsp_btn_ble.c) \
$(abspath $(SDK_ROOT)/components/ble/common/ble_advdata.c) \
$(abspath $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c) \
$(abspath $(SDK_ROOT)/components/ble/common/ble_conn_params.c) \
$(abspath $(SDK_ROOT)/components/ble/common/ble_srv_common.c) \
$(abspath $(SDK_ROOT)/components/ble/device_manager/device_manager_peripheral.c) \
$(abspath $(SDK_ROOT)/components/toolchain/system_nrf51.c) \
$(abspath $(SDK_ROOT)/components/softdevice/common/softdevice_handler/softdevice_handler.c) \

#assembly files common to all targets
ASM_SOURCE_FILES  = $(abspath $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf51.s)

#includes common to all targets

INC_PATHS += -I$(abspath $(SDK_ROOT)/components/libraries/util)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/drivers_nrf/pstorage)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/toolchain/gcc)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/toolchain)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/ble/common)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/drivers_nrf/common)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/ble/ble_advertising)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/drivers_nrf/config)
INC_PATHS += -I$(abspath $(SDK_ROOT)/examples/bsp)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/libraries/trace)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/drivers_nrf/gpiote)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/ble/device_manager)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/drivers_nrf/uart)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/libraries/uart)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/device)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/softdevice/common/softdevice_handler)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/softdevice/s110/headers)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/drivers_nrf/delay)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/libraries/timer)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/drivers_nrf/hal)
INC_PATHS += -I$(abspath $(SDK_ROOT)/components/libraries/button)

OBJECT_DIRECTORY = _build
LISTING_DIRECTORY = $(OBJECT_DIRECTORY)
OUTPUT_BINARY_DIRECTORY = $(OBJECT_DIRECTORY)

# Sorting removes duplicates
BUILD_DIRECTORIES := $(sort $(OBJECT_DIRECTORY) $(OUTPUT_BINARY_DIRECTORY) $(LISTING_DIRECTORY) )

ifeq ("$(DEBUG)","1")
    CFLAGS = -DDEBUG -g3
else
    CFLAGS = -DNDEBUG
endif

#flags common to all targets
CFLAGS += -DBOARD_PCA10028
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DNRF51
CFLAGS += -DS110
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DSWI_DISABLE0
CFLAGS += -mcpu=cortex-m0
CFLAGS += -mthumb -mabi=aapcs --std=gnu99
CFLAGS += -Wall -Werror -Os
CFLAGS += -mfloat-abi=soft
# keep every function in separate section. This will allow linker to dump unused functions
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin --short-enums

# keep every function in separate section. This will allow linker to dump unused functions
LDFLAGS += -Xlinker -Map=$(LISTING_DIRECTORY)/$(OUTPUT_FILENAME).map
LDFLAGS += -mthumb -mabi=aapcs -L $(TEMPLATE_PATH) -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m0
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs -lc -lnosys

# Assembler flags
ASMFLAGS += -x assembler-with-cpp
ASMFLAGS += -DBOARD_PCA10028
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DNRF51
ASMFLAGS += -DS110
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DSWI_DISABLE0
#default target - first one defined
default: clean nrf51422_xxac_s110

#building all targets

all: clean
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e cleanobj
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e nrf51422_xxac_s110

#target for printing all targets
help:
	@echo following targets are available:
	@echo 	nrf51422_xxac_s110
	@echo 	flash_softdevice


C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

ASM_SOURCE_FILE_NAMES = $(notdir $(ASM_SOURCE_FILES))
ASM_PATHS = $(call remduplicates, $(dir $(ASM_SOURCE_FILES) ))
ASM_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

nrf51422_xxac_s110: OUTPUT_FILENAME := nrf51422_xxac_s110
nrf51422_xxac_s110: LINKER_SCRIPT=config/ble_app_template_gcc_nrf51.ld
nrf51422_xxac_s110: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	$(NO_ECHO)$(MAKE) -f $(MAKEFILE_NAME) -C $(MAKEFILE_DIR) -e finalize

## Create build directories
$(BUILD_DIRECTORIES):
	echo $(MAKEFILE_NAME)
	$(MK) $@

# Create objects from C SRC files
$(OBJECT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

# Assemble files
$(OBJECT_DIRECTORY)/%.o: %.s
	@echo Compiling file: $(notdir $<)
	$(NO_ECHO)$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<


# Link
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out: $(BUILD_DIRECTORIES) $(OBJECTS)
	@echo Linking target: $(OUTPUT_FILENAME).out
	$(NO_ECHO)$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out


## Create binary .bin file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
$(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex: $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

finalize: genbin genhex echosize

genbin:
	@echo Preparing: $(OUTPUT_FILENAME).bin
	$(NO_ECHO)$(OBJCOPY) -O binary $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).bin

## Create binary .hex file from the .out file
genhex: 
	@echo Preparing: $(OUTPUT_FILENAME).hex
	$(NO_ECHO)$(OBJCOPY) -O ihex $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).hex

echosize:
	-@echo ''
	$(NO_ECHO)$(SIZE) $(OUTPUT_BINARY_DIRECTORY)/$(OUTPUT_FILENAME).out
	-@echo ''

clean:
	$(RM) $(BUILD_DIRECTORIES)

cleanobj:
	$(RM) $(BUILD_DIRECTORIES)/*.o

flash: $(MAKECMDGOALS)
	@echo Flashing: $(OUTPUT_BINARY_DIRECTORY)/$<.hex
	nrfjprog --program $(OUTPUT_BINARY_DIRECTORY)/$<.hex -f nrf51  --sectorerase
	nrfjprog --reset

## Flash softdevice
flash_softdevice:
	@echo Flashing: s110_nrf51_8.0.0_softdevice.hex
	nrfjprog --program $(SDK_ROOT)/components/softdevice/s110/hex/s110_nrf51_8.0.0_softdevice.hex -f nrf51 --chiperase
	nrfjprog --reset