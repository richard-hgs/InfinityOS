#include "ports.h"
#include "io.h"
#include "serial_com.h"

int serial_received(int port) {
    return inb(port + 5) & 1;
}
 
char read_serial(int port) {
   while (serial_received(port) == 0);
 
   return inb(port);
}

int is_transmit_empty(int port) {
   return inb(port + 5) & 0x20;
}
 
void serialcom_write(int port, char a) {
   while (is_transmit_empty(port) == 0);
 
   outb(port, a);
}

int serialcom_init_port(int port) {
    outb(port + 1, 0x00);    // Disable all interrupts
    outb(port + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(port + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(port + 1, 0x00);    //                  (hi byte)
    outb(port + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(port + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(port + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    outb(port + 4, 0x1E);    // Set in loopback mode, test the serial chip
    outb(port + 0, 0xAE);    // Test serial chip (send byte 0xAE and check if serial returns same byte)

    // Check if serial is faulty (i.e: not same byte as sent)
    if(inb(port + 0) != 0xAE) {
        return 1;
    }

    // If serial is not faulty set it in normal operation mode
    // (not-loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled)
    outb(port + 4, 0x0F);

    return 0;
}

void serialcom_test() {
    int port = COM_1_PORT;
    
    int initResult = serialcom_init_port(port);
    if (!initResult) {
        serialcom_write(port, 'A');
    } else {
        kprintf("%s %i %s\n", "Couldn't open serial port", 1, "for communication...");
    }
}