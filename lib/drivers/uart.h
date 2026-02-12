/********************************************************************
 * uart.h
 *  Serial port interface (UART)
 *  Author:  Erland Larsen
 *  Date:    2026-02-06
 *  Project: SPE4_API
 *  NOTICE:  Only support for 8 databit, 1 stop bit and no parity.
 *           Baud Rates supported: 2400, 4800, 9600, 14400, 19200,
 *           28800, 38400, 57600, 115200.
 *  Reservations:
 *           UART0: stdin, stdout, stderr are connected to UART0.
 *                  Do not use UART0 for other purposes if you want 
 *                  to use stdio functions for PC terminal.
 *           UART2: Used for WIFI module in AT-command mode.
 * 
  *******************************************************************/
#pragma once
#include <stdint.h>
#include "ringbuffer_static.h"

typedef enum {
    UART_OK = 0,
    UART_ERROR_INVALID_BAUD_RATE = -1,
    UART_ERROR_INIT_FAILED = -2,
    UART_ERROR_INVALID_ID = -3
} uart_t;

typedef enum {
    UART0_ID = 0,
    UART1_ID = 1,
    UART2_ID = 2,
    UART3_ID = 3
} uart_id_t;

typedef void (*rx_callback_t)(uint8_t byte);

// Initialize UART with the specified baud rate (e.g., 9600, 115200).
// Returns UART_OK on success, or negative error code on failure.
uart_t uart_init(uart_id_t uart_id, uint32_t baud_rate, rx_callback_t rx_callback, ringbuffer_t rx_buffer);
uart_t uart_write_bytes(const uint8_t* data, uint16_t length);
uart_t uart_write_byte(uart_id_t uart_id, int8_t b);
uint8_t uart_read_byte_blocking(uart_id_t uart_id);
