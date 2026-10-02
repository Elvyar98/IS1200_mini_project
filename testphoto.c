/* testphoto.c
 * DTEK-V 1-second polling of Phototransistors on GPIO#1 Pin 0 and Pin 1
 */

#define UART_DATA   ((volatile int *) 0x04000040)
#define UART_STATUS ((volatile int *) 0x04000044)

#define GPIO1_DATA  ((volatile int *) 0x040000E0)
#define GPIO1_DIR   ((volatile int *) 0x040000E4)

/* TMR1 registers */
#define TMR1_STAT   ((volatile unsigned short *) 0x04000020)
#define TMR1_CTRL   ((volatile unsigned short *) 0x04000024)
#define TMR1_PERLO  ((volatile unsigned short *) 0x04000028)
#define TMR1_PERHI  ((volatile unsigned short *) 0x0400002C)

/* Stubs required by boot.S */
void handle_interrupt(unsigned int cause) {}
void handle_exception(unsigned int cause) {}

/* Low-level UART character transmission */
void uart_putc(char c) {
    while (((*UART_STATUS >> 16) & 0xFFFF) == 0)
        ;
    *UART_DATA = c;
}

/* Transmit a null-terminated string */
void print(const char *str) {
    while (*str != '\0') {
        uart_putc(*str);
        str++;
    }
}

/* Configure GPIO#1 registers */
void gpio_init(void) {
    /* Set pins to input mode (0 = input) */
    *GPIO1_DIR = 0x0;
}

/* Read pins 0 and 1 from GPIO#1 (bits 1:0) */
int gpio_read_phototransistors(void) {
    return *GPIO1_DATA & 0x3;
}

/* Print pin 1 and pin 0 binary states, e.g. "P1: 1, P0: 0\n" */
void print_pins(int val) {
    int p0 = val & 0x1;
    int p1 = (val >> 1) & 0x1;

    print("P1: ");
    uart_putc(p1 ? '1' : '0');
    print(", P0: ");
    uart_putc(p0 ? '1' : '0');
    uart_putc('\n');
}

/* Configure TMR1 for a 1-second timeout (30 MHz clock) */
void timer_init(void) {
    unsigned int period = 30000000 - 1;

    *TMR1_PERLO = (unsigned short)(period & 0xFFFF);
    *TMR1_PERHI = (unsigned short)(period >> 16);

    *TMR1_STAT = 0;

    /* START (bit 2) = 1, CONT (bit 1) = 1, ITO (bit 0) = 0 -> 0x6 */
    *TMR1_CTRL = 0x6;
}

/* Wait until 1 second has elapsed, then reset the TO flag */
void wait_one_second(void) {
    while ((*TMR1_STAT & 0x1) == 0)
        ;
    *TMR1_STAT = 0;
}

/* Application entry */
int main(void) {
    gpio_init();
    timer_init();

    int last_val = gpio_read_phototransistors();

    print("Initial: ");
    print_pins(last_val);

    while (1) {
        wait_one_second();

        int current_val = gpio_read_phototransistors();

        if (current_val != last_val) {
            print("Changed: ");
            last_val = current_val;
        } else {
            print("No change: ");
        }
        print_pins(current_val);
    }

    return 0;
}
