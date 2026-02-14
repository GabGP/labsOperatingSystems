#ifndef OS_H
#define OS_H

/* OS Level - Hardware Interface (UART) */

void uart_putc(char c);
char uart_getc(void);
void uart_puts(const char *s);
void uart_gets_input(char *buffer, int max_length);

#endif /* OS_H */
