EXEC=test.elf
SRCS=main.c can.c

SYSTEM_SRCS = ../sys/startup_ARMCM4.c \
              ../sys/startup_clock.c

ASM_SRCS = 
SRCS += $(SYSTEM_SRCS)

#particular config:
OPTIM=0
CFLAGS += -I../sys/CMSIS/Device/ST/STM32F3xx/Include \
		  -I../sys/CMSIS/Include/
#LDFLAGS += 
EXTRA_FILES_CLEAN= $(EXEC) $(EXEC).bin $(EXEC).map

########################################################
# ARM SAMD21 related configuration
CXX=arm-none-eabi-gcc
CMSIS_FILES = ../sys/CMSIS

#flags
CFLAGS += -std=gnu99
CFLAGS += -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DSTM32F303x8 -DARMCM4
CFLAGS += -I$(CMSIS_FILES)/Device/ST/STM32F3xx/Include 
CFLAGS += -I$(CMSIS_FILES)/Include/
CFLAGS += -fstack-usage -ffunction-sections #TODO: a regarder plus en detail.

LDFLAGS += -mthumb -mcpu=cortex-m4 -Tstm32f303K8.ld -mfloat-abi=hard -mfpu=fpv4-sp-d16 --specs=nosys.specs -lm
LDFLAGS += -Wl,--start-group -lm  -Wl,--end-group -Wl,--gc-sections
LDFLAGS += -L../sys
LDFLAGS += -Xlinker -Map=$(EXEC).map

ALL: OBJ_DIR_CREATE $(EXEC)
	

#########################################################
## flash 
#########################################################
##extra rules (flash)

$(EXEC).bin: $(EXEC)
	arm-none-eabi-objcopy -O binary $(EXEC) $(EXEC).bin

flash: OBJ_DIR_CREATE $(EXEC).bin
	@st-flash --reset write $(EXEC).bin 0x8000000

# end of configuration (generic Makefile)...
########################################################
# compiler optimisation
# TODO: does not support optimisation level > 1
CFLAGS_DEBUG = -O0 -g -Wall -fexceptions -Wno-deprecated -DDEBUG
CFLAGS_OPTIM = -O3 -funroll-loops -fomit-frame-pointer -fno-strict-aliasing -pipe -ffast-math -fexceptions


ifeq ($(strip $(OPTIM)),1)
	CFLAGS += $(CFLAGS_OPTIM)
	msg += "with optimizations "
else
	CFLAGS += $(CFLAGS_DEBUG)
	msg += "without optimizations "
endif
########################################################

OBJ_DIR = build
SRCS_NODIR = $(notdir $(SRCS))
ASM_SRCS_NODIR = $(notdir $(ASM_SRCS))
#OBJ = $(addprefix $(OBJ_DIR)/,$(SRCS_NODIR:.cpp=.o))
OBJ = $(addprefix $(OBJ_DIR)/,$(SRCS_NODIR:.c=.o))
OBJ += $(addprefix $(OBJ_DIR)/,$(ASM_SRCS_NODIR:.S=.o))
#DEPFILES = $(addprefix $(OBJ_DIR)/,$(SRCS_NODIR:.cpp=.dep))
DEPFILES = $(addprefix $(OBJ_DIR)/,$(SRCS_NODIR:.c=.dep))

.PHONY: ALL
#ALL: OBJ_DIR_CREATE $(EXEC)

########################################################

.PHONY: clean OBJ_DIR_CREATE
clean:
	@rm -rf $(OBJ_DIR) *~  $(EXTRA_FILES_CLEAN)

$(EXEC): $(OBJ) 
	@echo linking.. $(EXEC)
	@$(CXX) -o $@ $(OBJ) $(LDFLAGS)

$(OBJ_DIR)/%.o: ../sys/%.c
	@echo c compiling $(msg)$< ...
	@$(CXX) -MD -MF $(OBJ_DIR)/$(notdir $<).dep -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.c
	@echo c compiling $(msg)$< ...
	@$(CXX) -MD -MF $(OBJ_DIR)/$<.dep -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.cpp 
	@echo c++ compiling $(msg)$< ...
	@$(CXX) -MD -MF $(OBJ_DIR)/$<.dep -c $(CFLAGS) $< -o $@

OBJ_DIR_CREATE: 
	@if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi;

#include dependancy files.
$(foreach source,$(SRC_NODIR),$(eval -include $(OBJ_DIR)/${source}.dep))

# vim:ft=make
