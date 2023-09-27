# STM32F411CEU6_BlackPill_Micropython
This port gives you a py-board-like experience using the BlackPill created by WeAct Studio.
There are 2 versions of Firmware provided, one for the 8MB version and one for the regular version. Both can be found in the `firmware` folder.
The 8MB version lets you use the 8MB spi storage to store your codes while the regular version offers 155KB flash storage for your codes.
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
make -j BOARD=WeAct_F411CE
```
There are 4 possible variants for the both of the boards: `DP`, `THREAD`, `DP_THREAD` and `NETWORK`. The `NETWORK` variant should not be used as it requires other components that has to be purchased separately.
```shell
# replace VARIANTS with DP, THREAD or DP_THREAD
# DP: Use double precision for floats, each float number takes twice as much memory.
# THREAD: Enables Thread. This is an experiemental feature
# DP_THREAD: Enables Thread and use double precision.
make -j BOARD=WeAct_F411CE BOARD_VARIANT=VARIANTS
```
