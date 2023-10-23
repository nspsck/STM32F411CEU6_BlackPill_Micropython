#define MICROPY_HW_BOARD_NAME       "WeAct Studio Blackpill v3.1 8MB"
#define MICROPY_HW_MCU_NAME         "STM32F411CE"

// Disabled network realted functions to save space
// Note STM32F411-series does not have ETH-interface
// #define MICROPY_PY_SOCKET                       (0)
// #define MICROPY_PY_NETWORK                      (0)
// #define MICROPY_PY_ONEWIRE                      (0)

// Enabling hardware related features
// For more details check mpconfigboard_common.h
#define MICROPY_HW_HAS_SWITCH       (1)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_HAS_MMA7660      (0)
#define MICROPY_HW_HAS_LCD          (0)
#define MICROPY_HW_ENABLE_RTC       (1)
#define MICROPY_HW_ENABLE_ADC       (1)
#define MICROPY_HW_ENABLE_USB       (1)
#define MICROPY_HW_ENABLE_SERVO     (1) // PA0-PA3, you might want to disable USART2
#define MICROPY_HW_ENABLE_TIMER     (1)
#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (0)

// HSE is 25MHz
#define MICROPY_HW_CLK_PLLM (25)
#define MICROPY_HW_CLK_PLLN (192)
#define MICROPY_HW_CLK_PLLP (RCC_PLLP_DIV2)
#define MICROPY_HW_CLK_PLLQ (4)
#define MICROPY_HW_CLK_LAST_FREQ (1)

// BlackPill has an optional 32kHz crystal
#define MICROPY_HW_RTC_USE_LSE      (1)
#define MICROPY_HW_RTC_USE_US       (0)
#define MICROPY_HW_RTC_USE_CALOUT   (1)

// External SPI Flash config
#if !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

#define MICROPY_HW_SPIFLASH_SIZE_BITS (64 * 1024 * 1024) // 64 Mbit (8 MByte)

#define MICROPY_HW_SPIFLASH_CS      (pin_A4)
#define MICROPY_HW_SPIFLASH_SCK     (pin_A5)
#define MICROPY_HW_SPIFLASH_MISO    (pin_A6)
#define MICROPY_HW_SPIFLASH_MOSI    (pin_A7)

extern const struct _mp_spiflash_config_t spiflash_config;
extern struct _spi_bdev_t spi_bdev;
#define MICROPY_HW_SPIFLASH_ENABLE_CACHE (1)
#define MICROPY_HW_BDEV_SPIFLASH    (&spi_bdev)
#define MICROPY_HW_BDEV_SPIFLASH_CONFIG (&spiflash_config)
#define MICROPY_HW_BDEV_SPIFLASH_SIZE_BYTES (MICROPY_HW_SPIFLASH_SIZE_BITS / 8)
#define MICROPY_HW_BDEV_SPIFLASH_EXTENDED (&spi_bdev) // for extended block protocol

#endif // !MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE

// UART config, 
// 12.5 Mbit/s for USART1 and USART6
// 6.25 Mbit/s for USART2
// Not defining the NAMEs reduces firmware size
// #define MICROPY_HW_UART1_NAME       "USART1"
#define MICROPY_HW_UART1_TX         (pin_A9) // Valid: PA9, PA15, PB6
#define MICROPY_HW_UART1_RX         (pin_A10) // Valid: PA10, PB3, PB7
// #define MICROPY_HW_UART2_NAME       "USART2"
#define MICROPY_HW_UART2_TX         (pin_A2) // Valid: PA2
#define MICROPY_HW_UART2_RX         (pin_A3) // Valid: PA3
// #define MICROPY_HW_UART6_NAME       "USART6"
// #define MICROPY_HW_UART6_TX         (pin_A11) // Valid: PA11
// #define MICROPY_HW_UART6_RX         (pin_A12) // Valid: PA12

#define MICROPY_HW_UART_REPL        PYB_UART_1
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C buses
// #define MICROPY_HW_I2C1_NAME        "I2C1"
#define MICROPY_HW_I2C1_SCL         (pin_B6) // Valid: PB6, PB8
#define MICROPY_HW_I2C1_SDA         (pin_B7) // Valid: PB7, PB9
// #define MICROPY_HW_I2C2_NAME        "I2C2"
#define MICROPY_HW_I2C2_SCL         (pin_B10) // Valid: PB10
#define MICROPY_HW_I2C2_SDA         (pin_B9) // Valid: PB3, PB9
// #define MICROPY_HW_I2C3_NAME        "I2C3"
#define MICROPY_HW_I2C3_SCL         (pin_A8) // Valid: PA8
#define MICROPY_HW_I2C3_SDA         (pin_B8) // Valid: PB4, PB8

// SPI buses
// If you use the SPI-Flash, the SPI1 will alredy be used.
// #define MICROPY_HW_SPI1_NAME        "SPI1"
#define MICROPY_HW_SPI1_NSS         (pin_A4) // Valid: PA4, PA15
#define MICROPY_HW_SPI1_SCK         (pin_A5) // Valid: PA5, PB3
#define MICROPY_HW_SPI1_MISO        (pin_A6) // Valid: PA6, PB4
#define MICROPY_HW_SPI1_MOSI        (pin_A7) // Valid: PA7, PB5
// #define MICROPY_HW_SPI2_NAME        "SPI2"
#define MICROPY_HW_SPI2_NSS         (pin_B12) // Valid: PB9, PB12
#define MICROPY_HW_SPI2_SCK         (pin_B13) // Valid: PB10, PB13 
#define MICROPY_HW_SPI2_MISO        (pin_B14) // Valid: PB14
#define MICROPY_HW_SPI2_MOSI        (pin_B15) // Valid: PB15
// #define MICROPY_HW_SPI3_NAME        "SPI3"
#define MICROPY_HW_SPI3_NSS         (pin_A15) // Valid: PA4, PA15
#define MICROPY_HW_SPI3_SCK         (pin_B3) // Valid: PB3, PB12
#define MICROPY_HW_SPI3_MISO        (pin_B4) // Valid: PB4
#define MICROPY_HW_SPI3_MOSI        (pin_B5) // Valid: PB5
// #define MICROPY_HW_SPI4_NAME        "SPI4"
// #define MICROPY_HW_SPI4_NSS         (pin_B12) // Valid: PB12
// #define MICROPY_HW_SPI4_SCK         (pin_B13) // Valid: PB13
// #define MICROPY_HW_SPI4_MISO        (pin_A11) // Valid: PA11
// #define MICROPY_HW_SPI4_MOSI        (pin_A1) // Valid: PA1
// #define MICROPY_HW_SPI5_NAME        "SPI5"
// #define MICROPY_HW_SPI5_NSS         (pin_B1) // Valid: PB1
// #define MICROPY_HW_SPI5_SCK         (pin_B0) // Valid: PB0
// #define MICROPY_HW_SPI5_MISO        (pin_A12) // Valid: PA12
// #define MICROPY_HW_SPI5_MOSI        (pin_A10) // Valid: PA10, PB8

// USRSW has no pullup or pulldown, and pressing the switch makes the input go low
#define MICROPY_HW_USRSW_PIN        (pin_A0)
#define MICROPY_HW_USRSW_PULL       (GPIO_PULLUP)
#define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_FALLING)
#define MICROPY_HW_USRSW_PRESSED    (0)

// The pyboard has 4 LEDs
#define MICROPY_HW_LED1             (pin_C13) // blue
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
#define MICROPY_HW_USB_FS              (1)
#define MICROPY_HW_FLASH_FS_LABEL   "blackpill"

#define MICROPY_HW_USB_MANUFACTURER_STRING      "WeActStudio"
#define MICROPY_HW_USB_PRODUCT_FS_STRING        "BlackPill v3.1 Comm Port (FS Mode)"
#define MICROPY_HW_USB_CONFIGURATION_FS_STRING  "BlackPill Config"
#define MICROPY_HW_USB_INTERFACE_FS_STRING      "BlackPill Interface"
// This is to save a tiny little space taken by strings.
#define MICROPY_HW_USB_PRODUCT_HS_STRING        ""
#define MICROPY_HW_USB_CONFIGURATION_HS_STRING  ""
#define MICROPY_HW_USB_INTERFACE_HS_STRING      ""
