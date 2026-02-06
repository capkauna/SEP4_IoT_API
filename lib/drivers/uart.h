/********************************************************************
 * uart.h
 *  Serial port interface (UART)
 *  Author:  Erland Larsen
 *  Date:    2026-02-06
 *  Project: SPE4_API
 * NOTICE:   stdin, stdout, stderr are connected to UART0.
 *           Do not use UART0 for other purposes if you want 
 *           to use stdio functions.
 *******************************************************************/
#pragma once
#include <stdint.h>

typedef enum {
    UART_OK = 0,
    UART_ERROR_INVALID_BAUD_RATE = -1,
    UART_ERROR_INIT_FAILED = -2
} uart_t;

typedef void (*rx_callback_t)(uint8_t byte);

// Initialize UART with the specified baud rate (e.g., 9600, 115200).
// Returns UART_OK on success, or negative error code on failure.
uart_t uart_init(uint32_t baud_rate, rx_callback_t rx_callback);
uart_t uart_send_bytes(const uint8_t* data, uint16_t length);
