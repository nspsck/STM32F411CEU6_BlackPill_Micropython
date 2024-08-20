# STM32F411CEU6_BlackPill_Micropython
**INDEX:**:
- [Pins definition](#pins-definition)
- [Lfs2 Support](#lfs2-support)
- [Storage modification](#storage-modification)
- [How to build](#how-to-build)
- [How to Flash](#how-to-flash)

This port gives you a py-board-like experience using the BlackPill created by WeAct Studio.

There are 2 major versions of the BlackPills, they differ only in terms of crystal speed. A total of 6 versions of Firmware are provided, 3 for the 8MHz crystal version and 3 for the regular 25MHz crystal version. They differ only in the SPI flash size (0MB, 8MB, and 16MB). All of them can be found in the `firmware` folder and they come in both `.dfu` and `.hex` format. You can also define your own SPI flash size as described in the `How to build` section.

The 8MHz version has the advantage of being more stable in terms of dfu-bootloader connectivity, as it tolerates a bigger margin of errors caused by ambient temperature.

A BlackPill with SPI flash has roughly 12KB more RAM available. This is due to the smaller cache required for the filesystem.

The pins/features-configurations are final, changes will only be done when forced by Micropython updates.

## Update 20.08.2024

A generic TFT-Driver has been included. Details and documentations [here](https://github.com/nspsck/st7735s_WeAct_Studio_TFT_port). Note the blackpills without SPI-Flash version are missing some fonts compared to the SPI-Flash version.

## Pins definition

Things that in `()` are available in the pre-built firmware but they might interfere with other interfaces, so be cautious. These are only my choices, you may use any other pins, which were stated in the comment as `Valid`. For the 8MB version, the `SPI1` bus is used for SPI flash. `SPI4` and `SPI5` are master out only by default, because both the `MISO` pins are used for USB.

| USART  | USART1 | USART2 |(USART6)|
| ------ | ------ | ------ | ------ |
| TX     | A9     | A2     | A11    |
| RX     | A10    | A3     | A12    |

| I2C  | I2C1 | I2C2 | I2C3 |
| ---- | ---- | ---- | ---- |
| SCL  | B6   | B10  | A8   |
| SDA  | B7   | B9   | B8   |

| SPI    | SPI1   | SPI2   | SPI3   | (SPI4) | (SPI5) |
| ------ | ------ | ------ | ------ | ------ | ------ |
| NSS    | A4     | B12    | A15    | B12    | B1     |
| SCK    | A5     | B13    | B3     | B13    | B0     |
| MISO   | A6     | B14    | B4     | N.A.   | N.A.   |
| MOSI   | A7     | B15    | B5     | A1     | A10    |

| LED  | KEY | USB_DM | USB_DP | SWDIO | SWCLK |
| ---- | --- | ------ | ------ | ----- | ----- |
| C13  | A0  | PA11   | PA12   | PA13  | PA14  |

## Lfs2 Support

`Lfs2` is a little fail-safe filesystem designed for microcontrollers to support dynamic wear leveling, which is quite useful when you are using the internal flash to log data. Since `FAT` does not have a wear leveling implementation, you will be wearing off your 10K cycles of the leading sectors quite fast.

To format the filesystem to `Lfs2`, you can simply execute the following code:

```python
import os, pyb
os.umount('/flash')
os.VfsLfs2.mkfs(pyb.Flash(start=0))
os.mount(pyb.Flash(start=0), '/flash')
os.chdir('/flash')
```

To avoid the annoying Windows formation message, add the following to `boot.py`:

```python
import pyb
pyb.usb_mode('VCP') # This will change the COM-number on Windows.
```

To go back to `FAT`:

```python
import os, pyb
os.umount('/flash')
os.VfsFat.mkfs(pyb.Flash(start=0))
os.mount(pyb.Flash(start=0), '/flash')
os.chdir('/flash')
```

To use a hybrid mode:
```python
import os, pyb
os.umount('/flash')
p1 = pyb.Flash(start=0, len=256*1024) # You have to caculate the length by your self.
p2 = pyb.Flash(start=256*1024) # You have to caculate the length by your self.
os.VfsFat.mkfs(p1)
os.VfsLfs2.mkfs(p2)
os.mount(p1, '/flash')
os.mount(p2, '/data')
os.chdir('/flash')
```


## Storage modification

### Flash layout (Only useful for the internal Flash version)
If changing the flash layout is of interest, you can modify the following section in the `stm32f411.ld` file. However, there is this one rule: **Never assign a sector of size more than your `FS_CACHE` size for `Flash_FS`.** Otherwise corruption will occur. This is caused by the fact that F4 series has very big sectors and every sector has to be erased before writing data to it. In case the `FS_CACHE` is not big enough to cache the biggest sector, when you are trying to write to the reset part of the sector, the sector itself will be erased but only part of its content was cached, as a result: you lost your uncached data and hence corruption.

The default one is almost optimized for the RAM and flash ratio. The following are 2 suggestions/examples of working modifications:

Example 1: Get a tiny little more flash (16K) at a cost of the same amount of RAM. You are using sectors 1, 2, 3 (each has 16K) and a part of sector 4 (which has 64K total, but only 32K of it is used) for your filesystem. The increased size can be anything between 0K to 32K (not including 0K and 32K). And you have to calculate the address accordingly.
```ld
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    /* This is your space for storing Code. */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 80K /* sectors 1,2,3,4: 16k+16k+16k+32K(of 64K)=80K */
    /* This is where the firmware is stored. */
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 384K /* sectors 5, 6, 7: 128K+128K+128K=384K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 96K
    FS_CACHE (xrw)  : ORIGIN = 0x20018000, LENGTH = 32K
}
```

Example 2: Maximizing flash size. You are using sectors 1, 2, 3 (each has 16K), and 4 (has 64K total) for your filesystem. This is the maximum amount of flash you can get at the cost of reducing the maximum firmware size by 128K and 64K of RAM. This is the max flash size, because for the next step, you need 128K RAM, and you only have 128K RAM on the stm32f411ce. Also, the maximum firmware size is reduced by 128K because when you erase sector 5, the part of the firmware stored on it will also be removed, causing the system to fail.
```ld
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    /* This is your space for storing Code. */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 176K /* sectors 1,2,3,4,5: 16k+16k+16k+64K+64K(of 128K)=176K */
    /* This is where the firmware is stored. */
    FLASH_TEXT (rx) : ORIGIN = 0x08040000, LENGTH = 256K /* sectors 6, 7: 128K+128K=256K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
    FS_CACHE (xrw)  : ORIGIN = 0x20010000, LENGTH = 64K
}
```
Note that you have to wipe the whole flash while flashing the firmware for the modifications to take effect.

### Using external SPI-Flash
You can either solder an external SPI-Flash directly on the board or connect the first SPI interface (`A4, A5, A6, A7`) to the external SPI-Flash module. Both ways you can use an e.g. `8MB` version of firmware directly. Assuming your external SPI-Flash has `8MB` of storage.
If the SPI-Flash is not wired/soldered correctly, the firmware still boots but you do not have access to the flash storage.

## How to build
### Vanilla
First of all, you have to have a Linux environment and get the compilers. On Ubuntu 23.10 and above or Debian 12 and above, it's called `gcc-arm-none-eabi`. On arch Linux, it's called `arm-none-eabi-gcc`.
```shell
sudo apt-get -y install gcc-arm-none-eabi build-essential # On Ubuntu or Debian
sudo pacman -S arm-none-eabi-gcc base-devel # On Arch Linux
```
After that, open the terminal and clone the official Micropython repo:
```shell
git clone https://github.com/micropython/micropython.git
cd micropython
git submodule update --init --recursive
cd mpy-cross
make -j
```
Copy `WEACTF411CE` to the `micropython/ports/stm32/boards` folder. To build the firmware, make sure you are in the `micropython/ports/stm32` folder, and type the following:
```shell
make -j LTO=1 BOARD=WEACTF411CE # With recent update, the LTO=1 can be omitted.
```
There are 4 possible variants for both of the base firmware: `DP`, `THREAD`, `DP_THREAD`, and `NETWORK`. The `NETWORK` variant requires other components that have to be purchased separately. And there are 2 variables that controls both the crystal frequency and the spi flash size, namely `CRYSTAL_FREQ` and `SPI_FLASH_SIZE`.
```shell
# replace VARIANTS with DP, THREAD or DP_THREAD
# DP: Use double precision for floats, each float number takes twice as much memory.
# THREAD: Enables Thread. This is an experimental feature
# DP_THREAD: Enables Thread and use double precision.
# CRYSTAL_FREQ: You can assign any value to this, if this keyword is defined, 
#               the 8MHz crystal firmware will be built. You can not make it
#               take the given value, because plli2svalues.py reads the .h files 
#               directly. (More of a reminder for myself.)
# SPI_FLASH_SIZE: You can set the SPI_FLASH_SIZE here to build your firmware accordingly.
# LTO: Whether to optimize for firmware size at the cost of some performance.
make -j LTO=1 BOARD=WEACTF411CE BOARD_VARIANT=VARIANTS CRYSTAL_FREQ=ANY_VALUE SPI_FLASH_SIZE=SIZE_IN_MB # With recent update, the LTO=1 can be omitted.
```
**Examples**:

For 8MHz crystal and 16MB spi flash:

`make -j BOARD=WEACTF411CE BOARD_VARIANT=VARIANTS CRYSTAL_FREQ=8 SPI_FLASH_SIZE=16`

For 25MHz crystal and 16MB spi flash, you have to leave `CRYSTAL_FREQ` out:

`make -j BOARD=WEACTF411CE BOARD_VARIANT=VARIANTS SPI_FLASH_SIZE=16`

### User C modules
If you wish to add some C modules, e.g. [a st7735 driver](https://github.com/nspsck/st7735s_WeAct_Studio_TFT_port/tree/main) you have to do the following:
```shell
make -j LTO=1 BOARD=WEACTF411CE USER_C_MODULES=~/lcd_c_modules/st7789mod/
```
change `~/lcd_c_modules/st7789mod/` to the directory where your modules are located at. Under that directory has to be a `microptyhon.cmake` file which includes includes all of the modules you want to have available. Something like this:
```cmake
include(${CMAKE_CURRENT_LIST_DIR}/st7789/micropython.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/example1/micropython.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/example2/micropython.cmake)
```
### Frozen contents
If you wish to freeze some `.py` modules to your firmware, e.g. some fonts files, you can do something like this:

- Step 1: Create a directory called `modules` under `ports/stm32`.
- Step 2: put all the `.py` files into the `modules` folder.
- Step 3: add the following line to the `manifest.py` under `ports/stm32/boards`.
```python
freeze("$(PORT_DIR)/modules")
```
Now you can just run the building command normally and everything in the `modules` folder will be frozen to the firmware.

## How to Flash

### Flashing the firmware
You can flash the firmware by following the official guide on i.e. [https://micropython.org/download/NUCLEO_F411RE/](https://micropython.org/download/NUCLEO_F411RE/) for both `.hex` and `.dfu` file.

### Mass erase

`st-flash`:
```shell
st-flash erase
```

`STM32CubeProgrammer`:
```shell
STM32_Programmer.sh -c port=SWD -e all
```

`dfu-util`:
```shell
# You can save the following as a file and run it with: sh file.sh
#!/bin/sh
set -x
echo -e -n "\xff" > ff.bin
dfu-util -s :mass-erase:force -a 0 -d 0483:df11 -D ff.bin
```

### STM32CubeProgrammer

In addition to that, if you were using `STM32CubeProgrammer` with a GUI, everything should be very straightforward. This is the easiest way to flash your device imo. There are 3 options available: `ST-Link, UART, and USB` each corresponding to using a `st-link`, using a `uart-bridge`, and using the built-in `dfu`.

### Side note

If you were unlucky and got a clone/pirated board, you might find it difficult to flash the firmware using `dfu`. (`st-link` and `uart-bridge` works just fine.) You can try to connect `A10` to the `GND` pin before you boot the board into dfu. This might solve your issue.

If you know for a fact, that your boards are official, try to heat the board up with a hairdryer and try again. This should solve your problem. 
