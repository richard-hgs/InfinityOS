/*
 * kernel.h - Header file for kernel.
 *
 * Author: Infinity Technology
 * Date  : 07/04/2020
 *
 *************************************************************************
 */

#ifndef _KERNEL_H_
#define _KERNEL_H_
#pragma once

#define WELCOME_MESSAGE \
" ___           __   _          _   _               ___    ___ \n" \
"|_ _|  _ _    / _| (_)  _ _   (_) | |_   _  _     / _ \\  / __|\n" \
" | |  | ' \\  |  _| | | | ' \\  | | |  _| | || |   | (_) | \\__ \\\n" \
"|___| |_||_| |_|   |_| |_||_| |_|  \\__|  \\_, |    \\___/  |___/\n" \
"                                         |__/                 \n" \
"\n" \
"Please login to the system.\n\nLOGIN? " \

/* Get user input and process it. */
void user_input(char *input);

#endif
