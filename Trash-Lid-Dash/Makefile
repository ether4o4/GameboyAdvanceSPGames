TARGET := TrashLidDash
BUILD := build
SOURCES := source
INCLUDES := source
GAME_TITLE := TRASHLIDDASH
GAME_CODE := TLD1
MAKER_CODE := WT

ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -Wextra -O2 -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer $(ARCH)
CFLAGS += $(INCLUDE)
ASFLAGS := $(ARCH)
LDFLAGS = -g $(ARCH) -Wl,-Map,$(notdir $@).map
LIBS := -lgba
LIBDIRS = $(LIBGBA)

include $(DEVKITARM)/gba_rules

ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
export LD := $(CC)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
export OFILES_SOURCES := $(CFILES:.c=.o)
export OFILES := $(OFILES_SOURCES)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                  -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: all $(BUILD) clean

all: $(BUILD)
	python3 tools/pad_gba.py $(TARGET).gba

$(BUILD):
	@mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@rm -rf $(BUILD) $(TARGET).elf $(TARGET).gba $(TARGET).elf.map

else

.PHONY: all
all: $(OUTPUT).gba

$(OUTPUT).gba: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)
$(OFILES_SOURCES):

-include $(DEPSDIR)/*.d

endif
