/***********************************************
 * uart_stdio.c
 *  Low-level UART stdio implementation
 *  Author:  Erland Larsen
 *  Date:    2026-01-30
 *  Project: SPE4_API
 **********************************************/
#include "uart_stdio.h"
#include "uart.h"
#include <avr/io.h>
#include <stdio.h>

static int uart0_putchar(char c, FILE *stream);
static int uart0_getchar(FILE *stream);

// Stream-objekt til stdio
static FILE uart0_stream;

void uart_stdio_init(uint32_t baud)
{
    uart_init(UART0_ID, baud, NULL);

    // Bind stdio til UART
    fdev_setup_stream(&uart0_stream, uart0_putchar, uart0_getchar, _FDEV_SETUP_RW);
    stdin  = &uart0_stream;
    stdout = &uart0_stream;
    stderr = &uart0_stream;
}

static int uart0_putchar(char c, FILE *stream)
{
    (void)stream;

    // Mange terminaler forventer CRLF
    if (c == '\n') {
        uart_write_byte(UART0_ID, '\r');
    }
    uart_write_byte(UART0_ID, (int8_t)c);
    return 0;
}

static int uart0_getchar(FILE *stream)
{
    (void)stream;

    int8_t c = uart_read_byte_blocking(UART0_ID);

    // Konverter CR til NL (så enter i terminal virker pænt)
    if (c == '\r') c = '\n';

    return (int)c;
}
