# STM32F411CEU6_BlackPill_Micropython

This port gives you a py-board-like experience using the BlackPill created by WeAct Studio.

There are 2 versions of Firmware provided, one for the 8MB version and one for the regular version. Both can be found in the `firmware` folder. Both in `.dfu` and `.hex` format.

The 8MB version lets you use the 8MB spi storage to store your codes while the regular version offers 47KB flash storage (or 63KB if you were using `Lfs2`) for your codes. The trade off is that you lose `SPI1`. Also, the 8MB version provides 12KB more RAM. The 8MB version can be directly used on a board with 16MB SPI-flash. (I do not understand why, but it works, magic. And by works I mean, it reports a 16MB flash.)

The pins/features-configurations are final, changes will only be done when forced by Micropython updates.

## Pins definition

Things that in `()` are not available in the pre-built firmware but they have their definitions in the `mpconfigboard.h` file. These are only my choices, you may use any other pins, which were sated in the comment as `Valid`. For the 8MB version, the `SPI1` bus is used for SPI flash.

| USART  | USART1 | USART2 | USART6 |
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
| MISO   | A6     | B14    | B4     | A11    | A12    |
| MOSI   | A7     | B15    | B5     | A1     | A10    |

| LED  | KEY | USB_DM | USB_DP | SWDIO | SWCLK |
| ---- | --- | ------ | ------ | ----- | ----- |
| C13  | A0  | PA11   | PA12   | PA13  | PA14  |

## Lfs2 Support

`Lfs2` is a little fail-safe filesystem designed for microcontrollers that supports dynamic wear leveling, which is quiet useful when you are using the internal flash to log datas, since `FAT` does not have a wear leveling implementation and you will be wearing off your 10K cycles quiet fast.

To format the filesystem to `Lfs2` you can simply excute the following code:

```python
import os, pyb
os.umount('/flash')
os.VfsLfs2.mkfs(pyb.Flash(start=0))
os.mount(pyb.Flash(start=0), '/flash')
os.chdir('/flash')
```

To avoid the annoying Windows formatation message, add the following to `boot.py`:

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

### Flash layout
If changing the flash layout is at your interest, you can modify the following section the `stm32f411.ld` file. However, there is this one rule: *never assign a part of a sector more than your `FS_CACHE` size for `Flash_FS`.* (Otherwise corruption will occur. This is caused by the fact that F4 series has very big sectors and each sectors greater than `FS_CACHE` can not be put into the cache.)

The default one is almost optimized (you can maybe still squeeze something out of the `FLASH_ISR` sector) for the RAM and flash ratio. The following are 2 suggestions/examples of working modifications:

Example 1: Get a tiny little more flash at cost of the same amount of RAM.
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

Example 2: Maximizing flash size. (not quite, you can still increase RAM usage for more flash, but at that point, it is just not worth it anymore)
```ld
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    /* This is your space for storing Code. */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 176K /* sectors 1,2,3,4,5: 16k+16k+16k+64K+64K(of 128K)=176K */
    /* This is where the firmware is stored. */
    FLASH_TEXT (rx) : ORIGIN = 0x08030000, LENGTH = 320K /* sectors 5, 6, 7: 64K(of 128K)+128K+128K=320K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 64K
    FS_CACHE (xrw)  : ORIGIN = 0x20010000, LENGTH = 64K
}
```
Note that you have to wipe the whole flash during flashing the firmware for the modifications to take effect.

### Using external SPI-Flash
You can either solder a external SPI-Flash directly on the board or connect the first SPI interface (`A4, A5, A6, A7`) to the external SPI-Flash module. Both way you can use the `8MB` version of firmware directly. Assuming your external SPI-Flash has `8MB` of storage.
If the SPI-Flash is not wired/soldered correctly, the firmware still boots but you do not have access to the flash storage.

## How to build
### Vanilla
First of all, you have to have a linux enviroment and get the compilers.
```shell
sudo apt-get -y install gcc-arm-none-eabi build-essential
```
After that, open the terminal and clone the official Micropython repo:
```shell
git clone https://github.com/micropython/micropython.git
cd micropython
git submodule update --init --recursive
cd mpy-cross
make -j
```
Copy `WEACTF411CE` or `WEACTF411CE8MB` to the `micropython/ports/stm32/boards` folder. To build the firmware, make sure you are in the `micropython/ports/stm32` folder, and type the following:
```shell
make -j LTO=1 BOARD=WEACTF411CE # With recent update, the LTO=1 can be omitted.
```
There are 4 possible variants for the both of the boards: `DP`, `THREAD`, `DP_THREAD` and `NETWORK`. The `NETWORK` variant should not be used as it requires other components that has to be purchased separately.
```shell
# replace VARIANTS with DP, THREAD or DP_THREAD
# DP: Use double precision for floats, each float number takes twice as much memory.
# THREAD: Enables Thread. This is an experiemental feature
# DP_THREAD: Enables Thread and use double precision.
make -j LTO=1 BOARD=WEACTF411CE BOARD_VARIANT=VARIANTS # With recent update, the LTO=1 can be omitted.
```
### User C modules
If you wish to add some C modules, e.g. [a st7735 driver](https://github.com/nspsck/st7735s_WeAct_Studio_TFT_port/tree/main) you have to do the following:
```shell
make -j LTO=1 BOARD=WEACTF411CE USER_C_MODULES=~/lcd_c_modules/st7789mod/ CFLAGS_EXTRA=-DMODULE_EXAMPLE_ENABLED=1 
```
change `~/lcd_c_modules/st7789mod/` to the directory where your modules are located at. Under that directory has to be a `microptyhon.cmake` file which includes includes all of the modules you want to have available. Something like this:
```cmake
include(${CMAKE_CURRENT_LIST_DIR}/st7789/micropython.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/example1/micropython.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/example2/micropython.cmake)
```
### Frozen contents
If you wish to freeze some `.py` modules to your firmware, e.g. some fonts files, you can do something like this:

- Step 1: create a directory called `modules` under `ports/stm32`.
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

In addtion to that, if you were using `STM32CubeProgrammer` with a GUI, everything should be very straight foward. And this is the most easy way to flash your device imo. There are 3 options available: `ST-Link, UART and USB` each corresponding to using a `st-link`, using a `uart-bridge` and using the built-in `dfu`.

### Side note

If you were unlucky and got a clone/pirated board, you might find it difficult to flash the firmware using `dfu`. (`st-link` and `uart-bridge` works just fine.) You can try to connect `A10` to the `GND` pin before you booting the board into dfu. This might solve your issue.
