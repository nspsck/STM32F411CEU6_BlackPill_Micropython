# STM32F411CEU6_BlackPill_Micropython

This port gives you a py-board-like experience using the BlackPill created by WeAct Studio.

There are 2 versions of Firmware provided, one for the 8MB version and one for the regular version. Both can be found in the `firmware` folder.

The 8MB version lets you use the 8MB spi storage to store your codes while the regular version offers 155KB flash storage for your codes. The trade of is that you lose `SPI1`.

The pins/features-configurations are final, changes will only be done when forced by Micropython updates.

## Pins definition

Things that in `()` are not available in the pre-built firmware but they have their definitions in the `mpconfigboard.h` file. These are only my choices, you may use any other pins, which were sated in the comment as `Valid`.

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

## Storage modification

If changing the flash layout is at your interest, you can modify the following section the `stm32f411.ld` file. 
```ld
MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K /* entire flash */
    FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K /* sector 0 */
    /* This is your space for storing Code. */
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 176K /* sectors 1,2,3,4,5: 16k+16k+16k+64K+64K(of 128K)=176K */
    /* This is where the firmware is stored. */
    FLASH_TEXT (rx) : ORIGIN = 0x08030000, LENGTH = 320K /* sectors 5, 6, 7: 64K(of 128K)+128K+128K=320K */
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 112K
    FS_CACHE (xrw)  : ORIGIN = 0x2001c000, LENGTH = 16K
}
```
Please make sure you modify the `TEXT0_ADDR` and `TEXT1_ADDR` in `mpconfigboard.mk` as well if you intend to use `.bin` files. Tho, you can also just delete these 2 lines and just use the `.hex` file.

## How to build

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
make -j LTO=1 BOARD=WeAct_F411CE
```
There are 4 possible variants for the both of the boards: `DP`, `THREAD`, `DP_THREAD` and `NETWORK`. The `NETWORK` variant should not be used as it requires other components that has to be purchased separately.
```shell
# replace VARIANTS with DP, THREAD or DP_THREAD
# DP: Use double precision for floats, each float number takes twice as much memory.
# THREAD: Enables Thread. This is an experiemental feature
# DP_THREAD: Enables Thread and use double precision.
make -j LTO=1 BOARD=WeAct_F411CE BOARD_VARIANT=VARIANTS
```
