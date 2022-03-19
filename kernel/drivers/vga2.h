#ifndef VGA_H
#define VGA_H
#pragma once
#include <stdint.h>
#include "keyboard.h"
/*
 * Some constans defined for vga 80 * 25 mode
 * */

// Some colors
#define BLACK           0
#define BLUE            1
#define GREEN           2
#define CYAN            3
#define RED             4
#define MAGENTA         5
#define BROWN           6
#define LIGHT_GREY      7
#define DARK_GREY       8
#define LIGHT_BLUE      9
#define LIGHT_GREEN     10
#define LIGHT_CYAN      11
#define LIGHT_RED       12
#define LIGHT_MAGENTA   13
#define LIGHT_BROWN     14
#define WHITE           15

/* Initialize the vga */
void vga_video_init();
/* Print ASCII string in screen */
void vga_print_string(char * s);
/* Print one ASCII character in screen */
void vga_print_char(int col, int row, int mVgaTextBufferCol, int mVgaTextBufferRow, char c);
/* Scroll screen down */
void vga_scroll_down(bool makeRoomForMore);
/* Scroll screen to the bottom */
void vga_scroll_bottom();
/* Scroll screen up */
void vga_scroll_up();
/* Get row from offset. */
int vga_get_offset_row(int offset);
/* Get column from offset. */
int vga_get_offset_col(int offset);
/* Get screen offset from (col, row). */
int vga_get_screen_offset(int col, int row);
/* Get cursor position */
void vga_get_cursor_position(int* col, int* row);
/* Set cursor position */
void vga_set_cursor_position(int col, int row);
/* Disable cursor */
void vga_disable_cursor();
/* Clear screen */
void vga_clear_screen();
/* Get max width */
int vga_get_max_width();
/* Get max height */
int vga_get_max_height();
/* When keyboard key is pressed */
void vga_on_key_pressed(KEYCODE keyCode);

void vga_test();

#endif