MCU_SERIES = f4
CMSIS_MCU = STM32F411xE
AF_FILE = boards/stm32f411_af.csv
LD_FILES = boards/WEACTF411CE/stm32f411.ld boards/common_ifs.ld
TEXT0_ADDR = 0x08000000
TEXT1_ADDR = 0x08030000

LTO ?= 1

# Micropython settings.
MICROPY_VFS_LFS2 = 1

# Provide different variants for the downloads page.
ifeq ($(BOARD_VARIANT),DP)
MICROPY_FLOAT_IMPL=double
endif

ifeq ($(BOARD_VARIANT),THREAD)
CFLAGS += -DMICROPY_PY_THREAD=1
endif

ifeq ($(BOARD_VARIANT),DP_THREAD)
MICROPY_FLOAT_IMPL=double
CFLAGS += -DMICROPY_PY_THREAD=1
endif

ifeq ($(BOARD_VARIANT),NETWORK)
MICROPY_PY_NETWORK_WIZNET5K=5200
endif

