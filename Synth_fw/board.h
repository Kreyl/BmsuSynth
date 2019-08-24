#pragma once

// ==== General ====
#define APP_NAME            "BmstuSynth"

// MCU type as defined in the ST header.
#define STM32L433xx

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ         12000000

// OS timer settings
#define STM32_ST_IRQ_PRIORITY   2
#define STM32_ST_USE_TIMER      2
#define SYS_TIM_CLK             (Clk.APB1FreqHz)    // Timer 5 is clocked by APB1

#define STM32_DMA_REQUIRED      TRUE    // Leave this macro name for OS

#if 1 // ========================== GPIO =======================================
// EXTI
#define INDIVIDUAL_EXTI_IRQ_REQUIRED    FALSE

// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     9
#define UART_RX_PIN     10

// UART485
#define UART485_GPIO        GPIOA
#define UART485_TX_PIN      2
#define UART485_RX_PIN      3
#define UART485_RXTX_PIN    GPIOA, 1, omPushPull

// SPI1
#define SPI1_CS         GPIOA, 4
#define SPI1_SCK        GPIOA, 5
#define SPI1_MISO       GPIOA, 6
#define SPI1_MOSI       GPIOA, 7
#define SPI1_AF         AF5

// SPI2
#define SPI2_CS         GPIOB, 12
#define SPI2_SCK        GPIOB, 13
#define SPI2_MISO       GPIOB, 14
#define SPI2_MOSI       GPIOB, 15
#define SPI2_AF         AF5

#endif // GPIO

#if 1 // =========================== DMA =======================================
// ==== Uart ====
// Remap is made automatically if required
#define UART_DMA_TX     STM32_DMA_STREAM_ID(2, 6)
#define UART_DMA_RX     STM32_DMA_STREAM_ID(2, 7)
#define UART_DMA_CHNL   2
#define UART_DMA_TX_MODE(Chnl) (STM32_DMA_CR_CHSEL(Chnl) | DMA_PRIORITY_LOW | STM32_DMA_CR_MSIZE_BYTE | STM32_DMA_CR_PSIZE_BYTE | STM32_DMA_CR_MINC | STM32_DMA_CR_DIR_M2P | STM32_DMA_CR_TCIE)
#define UART_DMA_RX_MODE(Chnl) (STM32_DMA_CR_CHSEL(Chnl) | DMA_PRIORITY_MEDIUM | STM32_DMA_CR_MSIZE_BYTE | STM32_DMA_CR_PSIZE_BYTE | STM32_DMA_CR_MINC | STM32_DMA_CR_DIR_P2M | STM32_DMA_CR_CIRC)

#define UART485_DMA_TX      STM32_DMA_STREAM_ID(1, 7)
#define UART485_DMA_RX      STM32_DMA_STREAM_ID(1, 6)
#define UART485_DMA_CHNL    2


#endif // DMA

#if 1 // ========================== USART ======================================
#define PRINTF_FLOAT_EN FALSE
#define UART_TXBUF_SZ   4096
#define UART_RXBUF_SZ   99

#define UARTS_CNT       2

#define CMD_UART_PARAMS \
    USART1, UART_GPIO, UART_TX_PIN, UART_GPIO, UART_RX_PIN, \
    UART_DMA_TX, UART_DMA_RX, UART_DMA_TX_MODE(UART_DMA_CHNL), UART_DMA_RX_MODE(UART_DMA_CHNL), \
    false // Use independent clock

#define CMD_UART485_PARAMS \
    USART2, UART485_GPIO, UART485_TX_PIN, UART485_GPIO, UART485_RX_PIN, \
    UART485_DMA_TX, UART485_DMA_RX, \
    UART_DMA_TX_MODE(UART485_DMA_CHNL), UART_DMA_RX_MODE(UART485_DMA_CHNL), \
    false // Use independent clock

#endif
