TARGET := TrashLidDash
BUILD := build
SOURCES := source
INCLUDES := source
GAME_TITLE := TRASHLIDDASH
GAME_CODE := TLD1
MAKER_CODE := WT

ARCH := -mthumb -mthumb-interwork
CFLAGS := -g -Wall -Wextra -O2 -mcpu=arm7tdmi -mtune=arm7tdmi -fomit-frame-pointer $(ARCH)
ASFLAGS := $(ARCH)
LDFLAGS = -g $(ARCH) -Wl,-Map,$(notdir $@).map
LIBS := -lgba
LIBDIRS := $(LIBGBA)

include $(DEVKITARM)/gba_rules
