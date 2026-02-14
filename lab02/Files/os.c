#include "os.h"

/* UART0 register definitions for VersatilePB */
#define UART0_BASE   0x101f1000
#define UART_DR      0x00  /* Data Register */
#define UART_FR      0x18  /* Flag Register */
#define UART_FR_TXFF 0x20  /* Transmit FIFO Full */
#define UART_FR_RXFE 0x10  /* Receive FIFO Empty */

volatile unsigned int * const UART0 = (unsigned int *)UART0_BASE;

/* Send a single character via UART */
void uart_putc(char c) {
    while (UART0[UART_FR / 4] & UART_FR_TXFF);
    UART0[UART_DR / 4] = c;
}

/* Receive a single character via UART */
char uart_getc(void) {
    while (UART0[UART_FR / 4] & UART_FR_RXFE);
    return (char)(UART0[UART_DR / 4] & 0xFF);
}

/* Send a null-terminated string via UART */
void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

/* Read a line of input via UART (with echo) */
void uart_gets_input(char *buffer, int max_length) {
    int i = 0;
    char c;
    while (i < max_length - 1) {
        c = uart_getc();
        if (c == '\n' || c == '\r') {
            uart_putc('\n');
            break;
        }
        uart_putc(c);
        buffer[i++] = c;
    }
    buffer[i] = '\0';
}
