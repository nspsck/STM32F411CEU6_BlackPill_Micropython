MCU_SERIES = f4
CMSIS_MCU = STM32F411xE
AF_FILE = boards/stm32f411_af.csv
# Provide different spi-flash size im MB support. From 2MB to 64MB.
# And set the LD_FILES accordinly.
ifndef SPI_FLASH_SIZE
CFLAGS += -DMICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE=1
LD_FILES = boards/WEACTF411CE/stm32f411.ld boards/common_ifs.ld
else
CFLAGS += -DMICROPY_PY_SPI_FLASH_SIZE=$(SPI_FLASH_SIZE)
CFLAGS += -DMICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE=0
LD_FILES = boards/WEACTF411CE/stm32f411_with_flash.ld boards/common_ifs.ld
endif

# Micropython settings.
MICROPY_VFS_LFS2 ?= 1

# Provide support for different crystal frequencies. Default: 25MHz
ifndef CRYSTAL_FREQ
    CFLAGS += -DMICROPY_PY_CRYSTAL_FREQ=25
else
    CFLAGS += -DMICROPY_PY_CRYSTAL_FREQ=8
endif
