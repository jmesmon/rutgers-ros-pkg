
#include "libarduino2.h"
#include <stdio.h>

#include "base_io.h"

void ros_io_init(void)
{
	serial_init();
}

static int uart_putchar(char c, FILE * stream)
{
	Serial.write(c);
	return 0;
}

static int uart_getchar(FILE * stream)
{
	return Serial.read();
}

static FILE ros_io_ = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW);
FILE *ros_io = &ros_io_;

