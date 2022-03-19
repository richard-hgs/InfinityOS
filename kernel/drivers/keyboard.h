/*
 * keyboard.h - Header file for keyboard driver.
 *
 * Author: Infinity Technology
 * Date  : 07/04/2020
 *
 ******************************************************************
 */

#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#pragma once

#include <stdbool.h>
#include <stdint.h>
// #include "types.h"

typedef enum KEYCODE {
	/* Alphanumeric keys */
	KEY_SPACE	= ' ',
	KEY_0		= '0',
	KEY_1		= '1',
	KEY_2		= '2',
	KEY_3		= '3',
	KEY_4		= '4',
	KEY_5		= '5',
	KEY_6		= '6',
	KEY_7		= '7',
	KEY_8		= '8',
	KEY_9		= '9',

	KEY_A		= 'a',
	KEY_B		= 'b',
	KEY_C		= 'c',
	KEY_D		= 'd',
	KEY_E		= 'e',
	KEY_F		= 'f',
	KEY_G		= 'g',
	KEY_H		= 'h',
	KEY_I		= 'i',
	KEY_J		= 'j',
	KEY_K		= 'k',
	KEY_L		= 'l',
	KEY_M		= 'm',
	KEY_N		= 'n',
	KEY_O		= 'o',
	KEY_P		= 'p',
	KEY_Q		= 'q',
	KEY_R		= 'r',
	KEY_S		= 's',
	KEY_T		= 't',
	KEY_U		= 'u',
	KEY_V		= 'v',
	KEY_W		= 'w',
	KEY_X		= 'x',
	KEY_Y		= 'y',
	KEY_Z		= 'z',

	KEY_RETURN	= '\r',
	KEY_ESCAPE	= 0x1001,
	KEY_BACKSPACE	= '\b',

	/* Arrow keys */
	KEY_UP		= 0x1100,
	KEY_DOWN	= 0x1101,
	KEY_LEFT	= 0x1102,
	KEY_RIGHT	= 0x1103,

	/* Function keys */
	KEY_F1		= 0x1201,
	KEY_F2		= 0x1202,
	KEY_F3		= 0x1203,
	KEY_F4		= 0x1204,
	KEY_F5		= 0x1205,
	KEY_F6		= 0x1206,
	KEY_F7		= 0x1207,
	KEY_F8		= 0x1208,
	KEY_F9		= 0x1209,
	KEY_F10	= 0x120a,
	KEY_F11	= 0x120b,
	KEY_F12	= 0x120c,
	KEY_F13	= 0x120d,
	KEY_F14	= 0x120e,
	KEY_F15	= 0x120f,

	KEY_DOT               = '.',
	KEY_COMMA             = ',',
	KEY_COLON             = ':',
	KEY_SEMICOLON         = ';',
	KEY_SLASH             = '/',
	KEY_BACKSLASH         = '\\',
	KEY_PLUS              = '+',
	KEY_MINUS             = '-',
	KEY_ASTERISK          = '*',
	KEY_EXCLAMATION       = '!',
	KEY_QUESTION          = '?',
	KEY_QUOTEDOUBLE       = '\"',
	KEY_QUOTE             = '\'',
	KEY_EQUAL             = '=',
	KEY_HASH              = '#',
	KEY_PERCENT           = '%',
	KEY_AMPERSAND         = '&',
	KEY_UNDERSCORE        = '_',
	KEY_LEFTPARENTHESIS   = '(',
	KEY_RIGHTPARENTHESIS  = ')',
	KEY_LEFTBRACKET       = '[',
	KEY_RIGHTBRACKET      = ']',
	KEY_LEFTCURL          = '{',
	KEY_RIGHTCURL         = '}',
	KEY_DOLLAR            = '$',
	KEY_POUND             = '£',
	KEY_EURO              = '$',
	KEY_LESS              = '<',
	KEY_GREATER           = '>',
	KEY_BAR               = '|',
	KEY_GRAVE             = '`',
	KEY_TILDE             = '~',
	KEY_AT                = '@',
	KEY_CARRET            = '^',

	/* Numeric keypad */
	KEY_KP_0              = '0',
	KEY_KP_1              = '1',
	KEY_KP_2              = '2',
	KEY_KP_3              = '3',
	KEY_KP_4              = '4',
	KEY_KP_5              = '5',
	KEY_KP_6              = '6',
	KEY_KP_7              = '7',
	KEY_KP_8              = '8',
	KEY_KP_9              = '9',
	KEY_KP_PLUS           = '+',
	KEY_KP_MINUS          = '-',
	KEY_KP_DECIMAL        = '.',
	KEY_KP_DIVIDE         = '/',
	KEY_KP_ASTERISK       = '*',
	KEY_KP_NUMLOCK        = 0x300f,
	KEY_KP_ENTER          = 0x3010,

	KEY_TAB               = 0x4000,
	KEY_CAPSLOCK          = 0x4001,

	/* Modify keys */
	KEY_LSHIFT            = 0x4002,
	KEY_LCTRL             = 0x4003,
	KEY_LALT              = 0x4004,
	KEY_LWIN              = 0x4005,
	KEY_RSHIFT            = 0x4006,
	KEY_RCTRL             = 0x4007,
	KEY_RALT              = 0x4008,
	KEY_RWIN              = 0x4009,

	KEY_INSERT            = 0x400a,
	KEY_DELETE            = 0x400b,
	KEY_HOME              = 0x400c,
	KEY_END               = 0x400d,
	KEY_PAGEUP            = 0x400e,
	KEY_PAGEDOWN          = 0x400f,
	KEY_SCROLLLOCK        = 0x4010,
	KEY_PAUSE             = 0x4011,

	KEY_UNKNOWN,
	KEY_NUMKEYCODES
} KEYCODE;

/* Get scroll lock on or off. */
extern char kbd_get_scrolllock(void);
/* Get num lock on or off. */
extern char kbd_get_numlock(void);
/* Get caps lock on or off. */
extern char kbd_get_capslock(void);
/* Return last scan code of pressed key. */
extern uint8_t kbd_get_last_scan(void);
/* Return last key that was pressed. */
extern KEYCODE kbd_get_last_key(void);
/* Discard last key that was received. */
extern void kbd_discard_last_key(void);
/* Convert key code into ascii value. */
extern char kbd_key_to_ascii(KEYCODE key);
/* Install keyboard handler. */
extern void install_kbd(void);
/** 
 * Set keyboard scancode set 
 * @param scanCodeSet 0x01, 0x02 or 0x03
 */
extern void kbd_set_scancode_set(unsigned char scanCodeSet);
/**
 * Get keyboard scancode set
 * @return  0x43, 0x41 or 0x3f
 */
extern char kbd_get_scancode_set(void);
/**
 * Sends a command to keyboard port
 */
extern void kbd_send_command(unsigned char command);

#endif
