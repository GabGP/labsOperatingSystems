#include "stdio.h"
#include "os.h"
#include <stdarg.h>
#include <stddef.h>

/* ------------------------------------------------------------------ */
/*  Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/* Convert integer to null-terminated string (adapted from uart_itoa) */
static void int_to_str(int num, char *buf) {
    int i = 0;
    int is_negative = 0;

    if (num == 0) {
        buf[i++] = '0';
        buf[i] = '\0';
        return;
    }

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }

    while (num > 0 && i < 14) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    if (is_negative) {
        buf[i++] = '-';
    }

    buf[i] = '\0';

    /* Reverse the string */
    int start = 0, end = i - 1;
    char temp;
    while (start < end) {
        temp = buf[start];
        buf[start] = buf[end];
        buf[end] = temp;
        start++;
        end--;
    }
}

/* Convert null-terminated string to integer (adapted from uart_atoi) */
static int str_to_int(const char *s) {
    int num = 0;
    int sign = 1;
    int i = 0;

    if (s[i] == '-') {
        sign = -1;
        i++;
    }

    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        num = num * 10 + (s[i] - '0');
    }

    return sign * num;
}

/* Convert float/double to null-terminated string with given decimal places */
static void float_to_str(double val, char *buf, int decimals) {
    int idx = 0;

    /* Handle negative */
    if (val < 0.0) {
        buf[idx++] = '-';
        val = -val;
    }

    /* Extract integer part */
    int int_part = (int)val;
    double frac_part = val - (double)int_part;

    /* Convert integer part to string (into a temp buffer, then copy) */
    char tmp[16];
    int_to_str(int_part, tmp);
    for (int t = 0; tmp[t] != '\0'; t++) {
        buf[idx++] = tmp[t];
    }

    /* Decimal point */
    buf[idx++] = '.';

    /* Fractional digits */
    for (int d = 0; d < decimals; d++) {
        frac_part *= 10.0;
        int digit = (int)frac_part;
        buf[idx++] = '0' + digit;
        frac_part -= (double)digit;
    }

    buf[idx] = '\0';
}

/* Convert null-terminated string to float */
static float str_to_float(const char *s) {
    float result = 0.0f;
    float sign = 1.0f;
    int i = 0;

    /* Handle sign */
    if (s[i] == '-') {
        sign = -1.0f;
        i++;
    }

    /* Integer part */
    for (; s[i] >= '0' && s[i] <= '9'; i++) {
        result = result * 10.0f + (float)(s[i] - '0');
    }

    /* Fractional part */
    if (s[i] == '.') {
        i++;
        float divisor = 10.0f;
        for (; s[i] >= '0' && s[i] <= '9'; i++) {
            result += (float)(s[i] - '0') / divisor;
            divisor *= 10.0f;
        }
    }

    return sign * result;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                         */
/* ------------------------------------------------------------------ */

/* PRINT - mimics printf; supports %d, %s, %f */
void PRINT(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    char buf[32];

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int val = va_arg(ap, int);
                    int_to_str(val, buf);
                    uart_puts(buf);
                    break;
                }
                case 's': {
                    const char *s = va_arg(ap, const char *);
                    uart_puts(s);
                    break;
                }
                case 'f': {
                    double val = va_arg(ap, double); /* variadic promotes float->double */
                    float_to_str(val, buf, 6);
                    uart_puts(buf);
                    break;
                }
                default:
                    /* Unknown specifier: print as-is */
                    uart_putc('%');
                    uart_putc(format[i]);
                    break;
            }
        } else {
            uart_putc(format[i]);
        }
    }

    va_end(ap);
}

/* READ - mimics scanf; supports %d, %f, %s */
void READ(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    char buf[64];

    for (int i = 0; format[i] != '\0'; i++) {
        if (format[i] == '%' && format[i + 1] != '\0') {
            i++;
            switch (format[i]) {
                case 'd': {
                    int *ptr = va_arg(ap, int *);
                    uart_gets_input(buf, sizeof(buf));
                    *ptr = str_to_int(buf);
                    break;
                }
                case 'f': {
                    float *ptr = va_arg(ap, float *);
                    uart_gets_input(buf, sizeof(buf));
                    *ptr = str_to_float(buf);
                    break;
                }
                case 's': {
                    char *ptr = va_arg(ap, char *);
                    uart_gets_input(ptr, 64);
                    break;
                }
                default:
                    break;
            }
        }
        /* Non-format characters in the format string are skipped */
    }

    va_end(ap);
}
