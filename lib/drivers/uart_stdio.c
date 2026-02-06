/***********************************************
 * uart_stdio.c
 *  Low-level UART stdio implementation
 *  Author:  Erland Larsen
 *  Date:    2026-01-30
 *  Project: SPE4_API
 **********************************************/
#include "uart_stdio.h"
#include <avr/io.h>
#include <stdio.h>

static int uart0_putchar(char c, FILE *stream);
static int uart0_getchar(FILE *stream);

// Stream-objekt til stdio
static FILE uart0_stream;

static inline uint16_t ubrr_from_baud(uint32_t baud)
{
    // Double speed (U2X0=1): UBRR = F_CPU/(8*baud) - 1
    return (uint16_t)((F_CPU / (8UL * baud)) - 1UL);
}

void uart_stdio_init(uint32_t baud)
{
    uint16_t ubrr = ubrr_from_baud(baud);
    if (ubrr == 7)
        ubrr = 8; // ugly hack because of rounding error..

    // Baudrate
    UBRR0H = (int8_t)(ubrr >> 8);
    UBRR0L = (int8_t)(ubrr & 0xFF);

    // Enable double speed
    UCSR0A = (1 << U2X0);
    
    // Enable RX + TX
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // 8N1
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Bind stdio til UART
    fdev_setup_stream(&uart0_stream, uart0_putchar, uart0_getchar, _FDEV_SETUP_RW);
    stdin  = &uart0_stream;
    stdout = &uart0_stream;
    stderr = &uart0_stream;
}

void uart0_write_byte(int8_t b)
{
    while (!(UCSR0A & (1 << UDRE0))) { }  // vent til buffer tom
    UDR0 = b;
}

int8_t uart0_read_byte_blocking(void)
{
    while (!(UCSR0A & (1 << RXC0))) { }   // vent på modtaget byte
    return UDR0;
}

static int uart0_putchar(char c, FILE *stream)
{
    (void)stream;

    // Mange terminaler forventer CRLF
    if (c == '\n') {
        uart0_write_byte('\r');
    }
    uart0_write_byte((int8_t)c);
    return 0;
}

static int uart0_getchar(FILE *stream)
{
    (void)stream;

    int8_t c = uart0_read_byte_blocking();

    // Konverter CR til NL (så enter i terminal virker pænt)
    if (c == '\r') c = '\n';

    return (int)c;
}
