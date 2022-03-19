/*
 * keyboard.c - Source file for simple keyboard input.
 *
 * Author: Philip R. Simonson
 * Date  : 07/04/2020
 *
 ******************************************************************
 */
#include <stdint.h>
#include "keyboard.h"
#include "kernel.h"
#include "system.h"
#include "helper.h"
#include "io.h"
#include "ports.h"
#include "isr.h"
#include "util.h"
#include "timer.h"

#define APPEND_KEYS(K, ...) K, ##__VA_ARGS__
#define BUILD_KBD_ERR(S, X) S = X

#define isascii(c) ((unsigned)(c) <= 0x7F)

/* Scan error codes */
enum KBD_ERROR {
	BUILD_KBD_ERR(KBD_ERR_BUF_OVERRUN, 0),
	BUILD_KBD_ERR(KBD_ERR_ID_RET, 0x83AB),
	BUILD_KBD_ERR(KBD_ERR_BAT, 0xAA),
	BUILD_KBD_ERR(KBD_ERR_ECHO_RET, 0xEE),
	BUILD_KBD_ERR(KBD_ERR_ACK, 0xFA),
	BUILD_KBD_ERR(KBD_ERR_BAT_FAILED, 0xFC),
	BUILD_KBD_ERR(KBD_ERR_DIAG_FAILED, 0xFD),
	BUILD_KBD_ERR(KBD_ERR_RESEND_CMD, 0xFE),
	BUILD_KBD_ERR(KBD_ERR_KEY, 0xFF)
};

extern char key_buffer[];
extern bool kbd_istyping;
int _scancode;

bool _kbd_bat_res;
bool _kbd_diag_res;
bool _kbd_resend_res;

bool _numlock;
bool _scrolllock;
bool _capslock;
bool _ctrl;
bool _shift;
bool _alt;
bool _extended;

uint64_t lastShiftMillis = 0;

// MAP in ordinal order
int _kbd_std_table[] = {
	//               00         01        02      03    04     05
	APPEND_KEYS(KEY_UNKNOWN, KEY_ESCAPE, KEY_1, KEY_2, KEY_3, KEY_4),
	//           06      07     08     09     0A     0B      0C
	APPEND_KEYS(KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_MINUS),
	//           0D              0E         F        10    11     12
	APPEND_KEYS(KEY_EQUAL, KEY_BACKSPACE, KEY_TAB, KEY_Q, KEY_W, KEY_E),
	//           13      14     15     16    17      18    19
	APPEND_KEYS(KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P),
	//                1A               1B             1C          1D 
	APPEND_KEYS(KEY_LEFTBRACKET, KEY_RIGHTBRACKET, KEY_RETURN, KEY_LCTRL),
	//            1E     1F     20    21     22     23     24      25     26
	APPEND_KEYS(KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L),
	//            27               28          29        2A           2B
	APPEND_KEYS(KEY_SEMICOLON, KEY_QUOTE, KEY_GRAVE, KEY_LSHIFT, KEY_BACKSLASH),
	//            2C    2D     2E      2F    30     31      32      33
	APPEND_KEYS(KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA),
	//            34         35         36            37             38
	APPEND_KEYS(KEY_DOT, KEY_SLASH, KEY_RSHIFT, KEY_KP_ASTERISK, KEY_RALT),
	//             39           3A         3B       3C     3D      3E      3F
	APPEND_KEYS(KEY_SPACE, KEY_CAPSLOCK, KEY_F1, KEY_F1, KEY_F2, KEY_F3, KEY_F4),
	//             40     41       42     43      44      45        46
	APPEND_KEYS(KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_SCROLLLOCK),
	//             47       48       49         4A             4B
	APPEND_KEYS(KEY_HOME, KEY_UP, KEY_PAGEUP, KEY_KP_MINUS, KEY_LEFT),
	//             4C        4D          4E          4F        50
	APPEND_KEYS(KEY_KP_5, KEY_RIGHT, KEY_KP_PLUS, KEY_END, KEY_DOWN),
	//             51             52          53          54           55           56         57       58
	APPEND_KEYS(KEY_PAGEDOWN, KEY_INSERT, KEY_DELETE, KEY_UNKNOWN, KEY_UNKNOWN, KEY_UNKNOWN, KEY_F11, KEY_F12)
};

/* Invalid scan code. Used to indicate the last scan code is not to be reused. */
const int INVALID_SCANCODE = 0;

/* Return scroll lock on/off.
 */
char kbd_get_scrolllock(void)
{
	return _scrolllock;
}
/* Return num lock on/off.
 */
char kbd_get_numlock(void)
{
	return _numlock;
}
/* Return caps lock on/off.
 */
char kbd_get_capslock(void)
{
	return _capslock;
}
/* Read Status from keyboard controller.
 */
char kbd_ctrl_read_status(void)
{
	return inb(0x64);
}
/* Send command byte to keyboard controller.
 */
void kbd_ctrl_send_cmd(unsigned char cmd)
{
	while(1) {
		if((kbd_ctrl_read_status() & 2) == 0)
			break;
		halt();
	}
	outb(0x64, cmd);
}
/* Read keyboard encoder buffer.
 */
char kbd_enc_read_buf(void)
{
	return inb(0x60);
}
/* Send command byte to keyboard encoder.
 */
void kbd_enc_send_cmd(unsigned char cmd)
{
	while(1) {
		if((kbd_ctrl_read_status() & 2) == 0)
			break;
		halt();
	}
	outb(0x60, cmd);
}
/* Set LEDs on keyboard for Special keys.
 */
void kbd_set_leds(bool num, bool caps, bool scroll)
{
	unsigned char data = 0;
	
	data = (scroll) ? (data | 1) : (data & 1);
	data = (num) ? (num | 2) : (num & 2);
	data = (caps) ? (num | 4) : (num & 4);
	
	kbd_enc_send_cmd(0xED);
	kbd_enc_send_cmd(data);
}

void disableShiftAfterTime() {
	if (_shift && (get_timer_ticks() - lastShiftMillis) > (uint64_t) 300) {
		_shift = false;
	}
}

void kbd_set_scancode_set(unsigned char scanCodeSet) {
	kbd_enc_send_cmd(0xF0);
	kbd_enc_send_cmd(scanCodeSet);
}

char kbd_get_scancode_set(void) {
	kbd_enc_send_cmd(0xF0);
	kbd_enc_send_cmd(0x00);
	char result = kbd_enc_read_buf();
	return result;
}

void kbd_send_cmd(unsigned char command) {
	kbd_ctrl_send_cmd(command);
}

/** 
 * Handle keyboard input from user.
 * Every time a key is pressed a make code is sent
 * Every time a key is released a break code is sent
 */
void keyboard_callback(registers_t* regs)
{
	// _extended = false;
	int code = 0;
	
	if(kbd_ctrl_read_status() & 1) {
		code = kbd_enc_read_buf();

		// kprintf("\nkeyboard -> code: %i or %x / %i or %x shift_state: %i or timerTicksB: %i\n", code, code, code + 0x80, code + 0x80, _shift, get_timer_ticks());

		if(code == 0xE0 || code == 0xE1) {
			// _extended = true;
		} else {
			// _extended = false;
			
			if(code & 0x80) {
				/* Convert the break to into it make code equivelant. */
				code += 0x80;
				
				int key = _kbd_std_table[code];
				switch(key) {
					case KEY_LCTRL:
					case KEY_RCTRL:
						_ctrl = false;
					break;
					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_shift = false;
					break;
					case KEY_LALT:
					case KEY_RALT:
						_alt = false;
					break;
					default:
						// Do nothing here
					break;
				}
			} else {
				/* This is a make code set the scan code. */
				_scancode = code;

				int key = _kbd_std_table[code];
				// kprintf("key: %i - %x\n", key, key);
				switch(key) {
					case KEY_LCTRL:
					case KEY_RCTRL:
						_ctrl = true;
					break;
					case KEY_LSHIFT:
					case KEY_RSHIFT:
						_shift = true;
					break;
					case KEY_LALT:
					case KEY_RALT:
						_alt = true;
					break;
					case KEY_CAPSLOCK:
						_capslock = (_capslock) ? false : true;
						kbd_set_leds(_numlock, _capslock, _scrolllock);
					break;
					case KEY_KP_NUMLOCK:
						_numlock = (_numlock) ? false : true;
						kbd_set_leds(_numlock, _capslock, _scrolllock);
					break;
					case KEY_SCROLLLOCK:
						_scrolllock = (_scrolllock) ? false : true;
						kbd_set_leds(_numlock, _capslock, _scrolllock);
					break;
					default:
						// Do nothing here
					break;
				}
			}
		
			/* Watch for errors. */
			switch(code) {
				case KBD_ERR_BAT_FAILED:
					_kbd_bat_res = false;
				break;
				case KBD_ERR_DIAG_FAILED:
					_kbd_diag_res = false;
				break;
				case KBD_ERR_RESEND_CMD:
					_kbd_resend_res = true;
				break;
				default:
					// Do nothing here
				break;
			}
		}
	}
	(void) regs;
}
/* Do a self test on the keyboard.
 */
char kbd_self_test(void)
{
	return true;
}
/* Return last scan code of pressed key.
 */
uint8_t kbd_get_last_scan(void)
{
	return _scancode;
}
/* Return last key that was pressed.
 */
KEYCODE kbd_get_last_key(void)
{
	return (_scancode != INVALID_SCANCODE) ? ((KEYCODE)_kbd_std_table[_scancode]) : KEY_UNKNOWN;
}
/* Discard last key that was received.
 */
void kbd_discard_last_key(void)
{
	_scancode = INVALID_SCANCODE;
}
/* Convert keycode to ascii value.
 */
char kbd_key_to_ascii(KEYCODE code)
{
	uint8_t key = code;
	
	if(isascii(key)) {
		if(_shift || _capslock) {
			if(key >= 'a' && key <= 'z') {
				key -= 32;
			}
		}
		
		if(_shift && !_capslock) {
			if(key >= '0' && key <= '9') {
				switch(key) {
					case '0':
						key = KEY_RIGHTPARENTHESIS;
					break;
					case '1':
						key = KEY_EXCLAMATION;
					break;
					case '2':
						key = KEY_AT;
					break;
					case '3':
						key = KEY_HASH;
					break;
					case '4':
						key = KEY_DOLLAR;
					break;
					case '5':
						key = KEY_PERCENT;
					break;
					case '6':
						key = KEY_CARRET;
					break;
					case '7':
						key = KEY_AMPERSAND;
					break;
					case '8':
						key = KEY_ASTERISK;
					break;
					case '9':
						key = KEY_LEFTPARENTHESIS;
					break;
					default:
						// Do nothing here
					break;
				}
			} else {
				switch(key) {
					case KEY_COMMA:
						key = KEY_GREATER;
					break;
					case KEY_DOT:
						key = KEY_LESS;
					break;
					case KEY_SLASH:
						key = KEY_QUESTION;
					break;
					case KEY_SEMICOLON:
						key = KEY_COLON;
					break;
					case KEY_QUOTE:
						key = KEY_QUOTEDOUBLE;
					break;
					case KEY_LEFTBRACKET:
						key = KEY_LEFTCURL;
					break;
					case KEY_RIGHTBRACKET:
						key = KEY_RIGHTCURL;
					break;
					case KEY_GRAVE:
						key = KEY_TILDE;
					break;
					case KEY_MINUS:
						key = KEY_UNDERSCORE;
					break;
					case KEY_EQUAL:
						key = KEY_PLUS;
					break;
					case KEY_BACKSLASH:
						key = KEY_BAR;
					break;
					default:
						// Do nothing here
					break;
				}
			}
		}
		
		return key;
	}
	
	return 0;
}
/* Initialize the keyboard.
 */
void install_kbd(void)
{
	register_interrupt_handler(IRQ1, keyboard_callback);
	
	_kbd_bat_res = kbd_self_test();
	_kbd_diag_res = _kbd_resend_res = false;
	
	kbd_istyping = false;
	_numlock = true;
	_scrolllock = _capslock = false;
	_shift = _ctrl = _alt = false;
	kbd_set_leds(_numlock, _capslock, _scrolllock);
}
