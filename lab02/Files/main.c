#include "stdio.h"

void main(void) {
    PRINT("Program: Add Two Numbers\n");

    while (1) {
        /* --- Integer addition --- */
        int num1, num2;

        PRINT("Enter first integer: ");
        READ("%d", &num1);

        PRINT("Enter second integer: ");
        READ("%d", &num2);

        PRINT("Sum: %d\n", num1 + num2);

        /* --- Floating-point addition --- */
        float f1, f2;

        PRINT("Enter first float: ");
        READ("%f", &f1);

        PRINT("Enter second float: ");
        READ("%f", &f2);

        PRINT("Sum: %f\n", (double)(f1 + f2));
    }
}
