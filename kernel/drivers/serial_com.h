#ifndef _SERIAL_COM_
#define _SERIAL_COM_
#pragma once

#define COM_1_PORT 0x3F8
#define COM_2_PORT 0x2F8
#define COM_3_PORT 0x3E8
#define COM_4_PORT 0x2E8

int serialcom_init_port(int port);

void serialcom_write(int port, char a);

void serialcom_test();

#endif