/***********************************************
 * uart_stdio.h
 *  Low-level UART stdio interface
 *  Author:  Erland Larsen
 *  Date:    2026-01-30
 *  Project: SPE4_API
 **********************************************/

#pragma once
#include <stdint.h>

// Initialiser USART0 og bind stdin/stdout/stderr til den
void uart_stdio_init(uint32_t baud);

// Valgfrit: direkte send/receive (kan bruges uden stdio)
void uart0_write_byte(int8_t b);
int8_t uart0_read_byte_blocking(void);

