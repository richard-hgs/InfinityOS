#include <stdbool.h>
#include <stddef.h>
#include "kstring.h"
#include "ports.h"
#include "io.h"
#include "memutils.h"

#include "vga2.h"

// VGA 80 * 25 mode screen
#define SCREEN ((uint16_t*)(0xB8000))
#define WIDTH 80
#define HEIGHT 25
#define DEFAULT_COLOR 0x0F
#define VGA_TEXT_BUFFER_PAGE_COUNT 20                                           // Must always be greather than 1
#define VGA_TEXT_BUFFER_SIZE WIDTH * HEIGHT * VGA_TEXT_BUFFER_PAGE_COUNT        // Buffer size must always be greather than vga screen size content
#define VGA_TEXT_BUFFER_SIZE_ROWS (HEIGHT * VGA_TEXT_BUFFER_PAGE_COUNT) - 1

#define MAKE_COLOR(bg, fg) ((bg << 4) | fg) 

// Make a pixel(2 bytes), background is always black, a is a char, b is foreground color
#define PAINT(a, bg, fg) (((MAKE_COLOR(bg, fg)) << 8) | (a & 0xFF))

// Get pixel
#define PIXEL(x, y) SCREEN[y * WIDTH + x]
#define PIXEL_BUFFER(x, y, buffer, value) buffer[y * WIDTH + x] = value; 

// int curr_x = 0, curr_y = 0;
uint8_t curr_bg_color = BLUE;
uint8_t curr_fg_color = WHITE;

// The buffer of the vga text
// Each char of vga is 16 bits long
uint16_t vgaTextBuffer[VGA_TEXT_BUFFER_SIZE];
int vgaTextBufferRow = 0;
int vgaTextBufferCol = 0;
int vgaTextBufferVerticalScrollPos = 0;
int vgaTextBufferVerticalScrollMax = 0;
int linesCleared = 0;

int savedCursorColPosition = -1;
int savedCursorRowPosition = -1;

int last_col = 0;

/*
 * Initialize
 * */
void vga_video_init() {
    // // Reset cursor positions
    // vga_set_cursor_position(0, 0);

    // Write blank chars in entire screen
    for (int row=0; row<HEIGHT; row++) {
        for (int col=0; col<WIDTH; col++) {
            PIXEL(col, row) = PAINT(0x20, curr_bg_color, curr_fg_color);
            PIXEL_BUFFER(col, row, vgaTextBuffer, PAINT(0x20, curr_bg_color, curr_fg_color));
        }
    }

    // Reset cursor positions again
    vga_set_cursor_position(0, 0);

    // kprintf("linesCleared: %i\n", linesCleared);

    // for (int i=0; i<HEIGHT + 2; i++) {
    //     kprintf("%i\n", i);
    // }

    // vga_scroll_up();
    // vga_scroll_up();
}

void vga_print_string(char* s) {
    while(*s != '\0') {
        vga_print_char(-1, -1, -1, -1, *s);
        s++;
    }
}

void vga_incrementVgaTextBufferRow(int* mVgaTextBufferRow) {
    if (vgaTextBufferRow < VGA_TEXT_BUFFER_SIZE_ROWS) {
        (*mVgaTextBufferRow)++;
    }
}

/* Print a char to screen, under current col, row position
 * scroll if necessary
 * For now, we only care about normal character, tab, and newline character
 */
void vga_print_char(int col, int row, int mVgaTextBufferCol, int mVgaTextBufferRow, char c) {
	extern bool kbd_istyping; // Defined inside keyboard.c

    if (savedCursorColPosition > -1 && savedCursorRowPosition > -1) {
        vga_scroll_bottom();
        col = -1;
        row = -1;
    }

    if (mVgaTextBufferCol == -1) {
        mVgaTextBufferCol = vgaTextBufferCol;
    }
    if (mVgaTextBufferRow == -1) {
        mVgaTextBufferRow = vgaTextBufferRow;
    }

    if (col < 0 || row < 0) {
        vga_get_cursor_position(&col, &row);
    }

    // if (row >= HEIGHT -1 && col >= WIDTH) {
    //     vga_scroll();
    //     vga_set_cursor_position(0, 24);
    // }

    if(c == '\n') { // Line feed
        if (vgaTextBufferRow >= (HEIGHT - 2)) {
            // Fill the rest of the previous buffer line with blank chars
            // dskprintf("Fill rest buffer %i\r\n", vgaTextBufferRow);

            for (int i=0; i<WIDTH-vgaTextBufferCol; i++) {
                PIXEL_BUFFER(vgaTextBufferCol + i, vgaTextBufferRow, vgaTextBuffer, PAINT(0x20, curr_bg_color, curr_fg_color));
            }
        }
        col = 0;
        mVgaTextBufferCol = 0;
        // if (vgaTextBufferEndContentRow < HEIGHT) {
        //     vgaTextBufferEndRow++;
        //     vgaTextBufferEndContentRow++;
        // }
        if (row < HEIGHT - 1) {
            row++;
            vga_incrementVgaTextBufferRow(&mVgaTextBufferRow);
            // if (linesCleared > 0) {
            //     linesCleared--;
            // }
        } else {
            vga_incrementVgaTextBufferRow(&mVgaTextBufferRow);
            vga_scroll_down(true);
        }
    } else if(c == '\t') {  // Tab
        for(int i = 0; i < 4; i++) {
            vga_print_char(-1, -1, -1, -1, ' ');
        }
        vga_get_cursor_position(&col, &row);
    } else if (c == '\b') {  // BackSpace
        if((row == 0 && col > 0) || (col >= 0 && last_col != 0)) {
            col -= 1;
            mVgaTextBufferCol -= 1;
            vga_print_char(col, row, mVgaTextBufferCol, mVgaTextBufferRow, ' ');
        }
    } else if (c >= ' ') {
        if(!kbd_istyping) {
			last_col = col;
        }
        PIXEL(col, row) = PAINT(c, curr_bg_color, curr_fg_color);
        PIXEL_BUFFER(mVgaTextBufferCol, mVgaTextBufferRow, vgaTextBuffer, PAINT(c, curr_bg_color, curr_fg_color));
        col++;
        mVgaTextBufferCol++;
        if(col == WIDTH) {
            col = 0;
            mVgaTextBufferCol = 0;
            // if (vgaTextBufferEndContentRow < HEIGHT) {
            //     vgaTextBufferEndRow++;
            //     vgaTextBufferEndContentRow++;
            // }
            if (row < HEIGHT - 1) {
                row++;
                // if (linesCleared > 0) {
                //     linesCleared--;
                // }
                vga_incrementVgaTextBufferRow(&mVgaTextBufferRow);
            } else {
                vga_incrementVgaTextBufferRow(&mVgaTextBufferRow);
                vga_scroll_down(true);
            }
        }
    }

    vga_set_cursor_position(col, row);
    vgaTextBufferCol = mVgaTextBufferCol;
    vgaTextBufferRow = mVgaTextBufferRow;
}

/* Scroll down by one line, copy the line 1 to line 0, line 2 to line 1...... and delete the last line */
void vga_scroll_down(bool makeRoomForMore) {
    if (makeRoomForMore || vgaTextBufferVerticalScrollPos < vgaTextBufferVerticalScrollMax) {
        // Copy all lines from vga memory up by one line
        void* start = (void*) (SCREEN + WIDTH);
        uint32_t size = (HEIGHT - 1) * WIDTH * 2;
        memcpy(SCREEN, start, size);
    }

    if (vgaTextBufferVerticalScrollPos == vgaTextBufferVerticalScrollMax) {
        if (makeRoomForMore) {
            // Make room for more
            if (vgaTextBufferRow < VGA_TEXT_BUFFER_SIZE_ROWS) {
                vgaTextBufferVerticalScrollMax++;
                vgaTextBufferVerticalScrollPos++;
            } else {
                // Vga text buffer is full copy all lines up by one line to make room for more content and discard old lines 
                void* start = (void*) (vgaTextBuffer + WIDTH);
                uint32_t size = (vgaTextBufferRow) * WIDTH * 2;
                memcpy(vgaTextBuffer, start, size);
                for (int col=0; col<WIDTH; col++) {
                    PIXEL_BUFFER(col, vgaTextBufferRow, vgaTextBuffer, PAINT(0x20, curr_bg_color, curr_fg_color));
                }
            }
        }
    } else {
        // Scrolling down
        if (vgaTextBufferVerticalScrollPos < vgaTextBufferVerticalScrollMax) {
            vgaTextBufferVerticalScrollPos++;
        }
    }

    // dskprintf("vS: %i - vSM: %i - bCol: %i - bRow: %i - lc: %i\r\n", vgaTextBufferVerticalScrollPos, vgaTextBufferVerticalScrollMax, vgaTextBufferCol, vgaTextBufferRow, linesCleared);


    if (makeRoomForMore) {
        vga_set_cursor_position(0, 24);
        for (int col=0; col<WIDTH; col++) {
            PIXEL(col, 24) = PAINT(0x20, curr_bg_color, curr_fg_color);
        }
    } else if (vgaTextBufferVerticalScrollPos <= vgaTextBufferVerticalScrollMax) {
        // Scroll content down
        // Copy old last buffer line to vga screen memory content
        if (vgaTextBufferVerticalScrollPos < vgaTextBufferVerticalScrollMax) {
            vga_set_cursor_position(0, 24);
        }

        bool scrollGreatherThanVgaHeight = (linesCleared > 0 && vgaTextBufferRow > (HEIGHT - 1) && (vgaTextBufferVerticalScrollPos + (HEIGHT - 1) <= vgaTextBufferRow));


        // vsM: 31 - vs: 29 = 2
        // br: 31 - lc: 29 = 2
        // 

        // int lcrpy = 0;
        // if (scrollGreatherThanVgaHeight) {
        //     lcrpy = vgaTextBufferVerticalScrollPos + (HEIGHT - 1);
        // }

        if (linesCleared > 0 && vgaTextBufferVerticalScrollPos + linesCleared > vgaTextBufferVerticalScrollMax) {
            for (int col=0; col<WIDTH; col++) {
                PIXEL(col, 24) = PAINT(0x20, curr_bg_color, curr_fg_color);
            }
        }

        if (
            (
                linesCleared == 0 && (
                    (vgaTextBufferVerticalScrollPos < vgaTextBufferVerticalScrollMax) || 
                    (vgaTextBufferVerticalScrollPos == vgaTextBufferVerticalScrollMax && (savedCursorColPosition != -1 && savedCursorRowPosition != -1))
                )
            ) || scrollGreatherThanVgaHeight
        ) {
            
            // void* vgaStart = (void*) (vgaTextBuffer + (vgaTextBufferRow - (vgaTextBufferRow > (HEIGHT - 1) ? ((vgaTextBufferVerticalScrollMax - (scrollGreatherThanVgaHeight ? linesCleared : 0)) - vgaTextBufferVerticalScrollPos) : 0)) * WIDTH);
            
            int lastBufferRowPos = vgaTextBufferRow - (vgaTextBufferVerticalScrollMax - vgaTextBufferVerticalScrollPos);
            if (scrollGreatherThanVgaHeight) {
                lastBufferRowPos = vgaTextBufferVerticalScrollPos + (HEIGHT - 1);

                if (lastBufferRowPos == vgaTextBufferRow) {
                    // Last line
                    for (int i=0; i<WIDTH-vgaTextBufferCol; i++) {
                        PIXEL_BUFFER(vgaTextBufferCol + i, vgaTextBufferRow, vgaTextBuffer, PAINT(0x20, curr_bg_color, curr_fg_color));
                        // PIXEL_BUFFER(col + i, (row < HEIGHT - 1 ? row : vgaTextBufferEndContentRow - 1), vgaTextBuffer, PAINT(0x20, curr_bg_color, curr_fg_color));
                    }
                }
            }

            void* vgaOut = (void*) (SCREEN + (HEIGHT - 1) * WIDTH);
            void* vgaStart = (void*) (vgaTextBuffer + lastBufferRowPos * WIDTH);

            // if ((vgaTextBufferRow > (HEIGHT - 1) ? ((vgaTextBufferVerticalScrollMax - (scrollGreatherThanVgaHeight ? linesCleared : 0)) - vgaTextBufferVerticalScrollPos) : 0) > 0) {
            // dskprintf("vS: %i - vSM: %i - bCol: %i - bRow: %i - lc: %i - sg: %i - lrcpy: %i\r\n", vgaTextBufferVerticalScrollPos, vgaTextBufferVerticalScrollMax, vgaTextBufferCol, vgaTextBufferRow, linesCleared, scrollGreatherThanVgaHeight, lastBufferRowPos);

            memcpy(vgaOut, vgaStart, WIDTH * 2);
        }

        if (vgaTextBufferVerticalScrollPos == vgaTextBufferVerticalScrollMax && savedCursorColPosition != -1 && savedCursorRowPosition != -1) {
            vga_set_cursor_position(savedCursorColPosition, savedCursorRowPosition);
            // Fill the rest of the last line with blank chars
            size_t startCol = savedCursorColPosition;
            for (int col=startCol; col<WIDTH; col++) {
                PIXEL(col, 24) = PAINT(0x20, curr_bg_color, curr_fg_color);
            }
            savedCursorColPosition = -1;
            savedCursorRowPosition = -1;
        }
    }

    // bool shouldLoadNext = false;
    // // Screen is full scroll content down to open room for more text
    // if (makeRoomForMore || (vgaTextBufferEndContentRow - vgaTextBufferEndRow) > 0) {
    //     if (!makeRoomForMore || (vgaTextBufferEndContentRow * WIDTH) < VGA_TEXT_BUFFER_SIZE) {
    //         shouldLoadNext = true;
    //         vgaTextBufferStartRow++;
    //         vgaTextBufferEndRow++;
    //     }
    // }

    // if (makeRoomForMore) {
    //     if ((vgaTextBufferEndContentRow * WIDTH) < VGA_TEXT_BUFFER_SIZE) {
    //         vgaTextBufferEndContentRow++;
    //     } else {
    //         // Vga text buffer is full copy all lines up by one line to make room for more content and discard old lines 
    //         void* start = (void*) (vgaTextBuffer + WIDTH);
    //         uint32_t size = (vgaTextBufferEndContentRow - 1) * WIDTH * 2;
    //         memcpy(vgaTextBuffer, start, size);
    //     }
    // }

    // if (makeRoomForMore || shouldLoadNext) {
    //     // Copy all lines from vga memory up by one line
    //     void* start = (void*) (SCREEN + WIDTH);
    //     uint32_t size = (HEIGHT - 1) * WIDTH * 2;
    //     memcpy(SCREEN, start, size);

    //     // Clear the last line
    //     if (makeRoomForMore) {
    //         vga_set_cursor_position(0, 24);
    //         for (int col=0; col<WIDTH; col++) {
    //             PIXEL(col, 24) = PAINT(0x20, curr_bg_color, curr_fg_color);
    //         }
    //     } else {
    //         // Copy old last buffer line to vga screen memory content
    //         int height = HEIGHT - 1;
    //         // if (vgaTextBufferEndContentRow < height) {
    //         //     height = vgaTextBufferEndContentRow;
    //         // }

    //         void* vgaOut = (void*) (SCREEN + height * WIDTH);
    //         void* vgaStart = (void*) (vgaTextBuffer + (vgaTextBufferEndRow - 1) * WIDTH);
    //         memcpy(vgaOut, vgaStart, WIDTH * 2);
            
    //         if ((vgaTextBufferEndContentRow - vgaTextBufferEndRow) == 0) {
    //             // Fill the rest of the last line with blank chars to fix the black line issue
    //             vga_set_cursor_position(savedCursorColPosition, savedCursorRowPosition);
    //             size_t startCol = savedCursorColPosition;
    //             for (int col=startCol; col<WIDTH; col++) {
    //                 PIXEL(col, 24) = PAINT(0x20, curr_bg_color, curr_fg_color);
    //             }
    //             vga_set_cursor_position(savedCursorColPosition, savedCursorRowPosition);
    //             savedCursorColPosition = -1;
    //             savedCursorRowPosition = -1;
    //         }
    //     }
    // }
}

void vga_scroll_bottom() {
    // Copy old last buffer line to vga screen memory content
    int height = HEIGHT;
    if (vgaTextBufferRow < height) {
        height = vgaTextBufferRow;
    } else {
        height = savedCursorRowPosition + 1;
    }

    void* vgaStart = (void*) (vgaTextBuffer + (vgaTextBufferRow - (height == HEIGHT ? height - 1 : 0)) * WIDTH);
    memcpy(SCREEN, vgaStart, height * WIDTH * 2);

    vgaTextBufferVerticalScrollPos = vgaTextBufferVerticalScrollMax;
    
    // Fill the rest of the last line with blank chars to fix the black line issue
    if (height < HEIGHT) {
        int currentColPosition = 0;
        int currentRowPosition = 0;
        vga_get_cursor_position(&currentColPosition, &currentRowPosition);
        for (int row=savedCursorRowPosition; row<(HEIGHT - savedCursorRowPosition); row++) {
            for (int col=(row == savedCursorRowPosition ? savedCursorColPosition : 0); col<WIDTH; col++) {
                PIXEL(col, row) = PAINT(0x20, curr_bg_color, curr_fg_color);
            }
        }
        vga_set_cursor_position(savedCursorColPosition, savedCursorRowPosition);
        savedCursorColPosition = -1;
        savedCursorRowPosition = -1;
    } else {
        vga_set_cursor_position(savedCursorColPosition, savedCursorRowPosition);
        for (int col=savedCursorColPosition; col<WIDTH; col++) {
            PIXEL(col, 24) = PAINT(0x20, curr_bg_color, curr_fg_color);
        }
        vga_set_cursor_position(savedCursorColPosition, savedCursorRowPosition);
        savedCursorColPosition = -1;
        savedCursorRowPosition = -1;
    }
}

void vga_scroll_up() {
    // kprintf("scrollUp - vS: %i - vSM: %i - bCol: %i - bRow: %i - lc: %i\r\n", vgaTextBufferVerticalScrollPos, vgaTextBufferVerticalScrollMax, vgaTextBufferCol, vgaTextBufferRow, linesCleared);

    if (vgaTextBufferVerticalScrollPos > 0) {
        vgaTextBufferVerticalScrollPos--;

        // Save cursor position for the next input text
        if (savedCursorColPosition == -1 && savedCursorRowPosition == -1) {
            vga_get_cursor_position(&savedCursorColPosition, &savedCursorRowPosition);
        }

        // Copy all lines from bottom to up
        void* out = (void*) (SCREEN + HEIGHT * WIDTH);
        void* start = (void*) (SCREEN + (HEIGHT - 1) * WIDTH);
        uint32_t size = HEIGHT * WIDTH * 2;
        memcpy_r(out, start, size);

        for (int col=0; col<WIDTH; col++) {
            PIXEL(col, 0) = PAINT(0x20, curr_bg_color, curr_fg_color);
        }

        // Copy previous buffer line to the first line of the vga buffer on screen
        void* vgaStart = (void*) (vgaTextBuffer + vgaTextBufferVerticalScrollPos * WIDTH);
        memcpy(SCREEN, vgaStart, WIDTH * 2);
        
        // There is a error ins scroll_up buffer
    }
}

/* Get row from cursor offset */
int vga_get_offset_row(int offset) {
	return offset / WIDTH;
}
/* Get column from cursor offset */
int vga_get_offset_col(int offset) {
    // y = pos / VGA_WIDTH; x = pos % VGA_WIDTH;
	return offset % WIDTH;
}

/* Get offset for screen (col, row) */
int vga_get_screen_offset(int col, int row) {
	return row * WIDTH + col;
}

void vga_get_cursor_position(int* col, int* row) {
    uint16_t offset = 0;
    outb(0x3D4, 0x0F);
    offset = offset | inb(0x3D5);
    outb(0x3D4, 0x0E);
    offset = offset | ((uint16_t)inb(0x3D5)) << 8;

    *col = vga_get_offset_col(offset);
    *row = vga_get_offset_row(offset);
}

/*
 * Update cursor
 */
void vga_set_cursor_position(int col, int row) {
    unsigned offset_pos = vga_get_screen_offset(col, row);

    outb(0x3D4, 14);
    outb(0x3D5, offset_pos >> 8);
    outb(0x3D4, 15);
    outb(0x3D5, offset_pos);
}

/* Disable cursor */
void vga_disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

/* Clear screen */
void vga_clear_screen() {
    // Clear screen
    // memsetw(SCREEN, PAINT(0x20, curr_bg_color, curr_fg_color), WIDTH * HEIGHT / 2);

    // Get cursor position
    int cursorCol = 0;
    int cursorRow = 0;
    vga_get_cursor_position(&cursorCol, &cursorRow);

    // dskprintf("Before cls - vS: %i - vSM: %i - bCol: %i - bRow: %i - lc: %i\r\n", vgaTextBufferVerticalScrollPos, vgaTextBufferVerticalScrollMax, vgaTextBufferCol, vgaTextBufferRow, linesCleared);


    for (int i=0; i<cursorRow; i++) {
        vga_scroll_down(true);
        for (int x=0;x<WIDTH; x++) {
            PIXEL_BUFFER(x, vgaTextBufferRow, vgaTextBuffer, PAINT(0x20, curr_bg_color, curr_fg_color));
        }
    }
    vga_set_cursor_position(0, 0);
    linesCleared += cursorRow;

    // dskprintf("After cls - vS: %i - vSM: %i - bCol: %i - bRow: %i - lc: %i\r\n", vgaTextBufferVerticalScrollPos, vgaTextBufferVerticalScrollMax, vgaTextBufferCol, vgaTextBufferRow, linesCleared);


    // vgaTextBufferEndRow = vgaTextBufferEndContentRow + (HEIGHT - cursorRow);
    // vgaTextBufferEndContentRow = vgaTextBufferEndRow;

    // for (int i=0; i<cursorRow; i++) {
    //     vga_scroll_down(true);
    //     for (int x=0;x<WIDTH; x++) {
    //         PIXEL_BUFFER(x, vgaTextBufferEndContentRow, vgaTextBuffer, PAINT(0x20, curr_bg_color, curr_fg_color));
    //     }
    // }

    // vga_set_cursor_position(0, 0);
}

int vga_get_max_width() {
    return WIDTH - 1;
}

int vga_get_max_height() {
    return HEIGHT - 1;
}

void vga_on_key_pressed(KEYCODE keyCode) {
    if(keyCode == KEY_LEFT) {
        // // Arrow left pressed
        // int currentCursorCol = 0;
        // int currentCursorRow = 0;

        // vga_get_cursor_position(&currentCursorCol, &currentCursorRow);

        // if (savedCursorColPosition == -1 && savedCursorRowPosition == -1) {
        //     savedCursorColPosition = currentCursorCol;
        //     savedCursorRowPosition = currentCursorRow;
        // }

        // if (currentCursorCol > 0) {
        //     currentCursorCol--;
        // } else {
        //     if (currentCursorRow > 0) {
        //         currentCursorCol = vga_get_max_width();
        //         currentCursorRow--;
        //     } else {
        //         currentCursorCol = WIDTH - 1;
        //         vga_scroll_up();
        //     }
        // }

        // vga_set_cursor_position(currentCursorCol, currentCursorRow);
    } else if (keyCode == KEY_UP) {
        // // Arrow up pressed
        // int currentCursorCol = 0;
        // int currentCursorRow = 0;

        // vga_get_cursor_position(&currentCursorCol, &currentCursorRow);

        // if (savedCursorColPosition == -1 && savedCursorRowPosition == -1) {
        //     savedCursorColPosition = currentCursorCol;
        //     savedCursorRowPosition = currentCursorRow;
        // }

        // if (currentCursorRow > 0) {
        //     currentCursorRow--;
        // } else {
        //     vga_scroll_up();
        // }

        // vga_set_cursor_position(currentCursorCol, currentCursorRow);
    } else if (keyCode == KEY_DOWN) {
        // Arrow down pressed
        // int currentCursorCol = 0;
        // int currentCursorRow = 0;

        // vga_get_cursor_position(&currentCursorCol, &currentCursorRow);

        // if (savedCursorColPosition == -1 && savedCursorRowPosition == -1) {
        //     savedCursorColPosition = currentCursorCol;
        //     savedCursorRowPosition = currentCursorRow;
        // }

        // if (currentCursorRow < vga_get_max_height()) {
        //     currentCursorRow++;
        // } else {
        //     vga_scroll_down(false);
        //     vga_get_cursor_position(&currentCursorCol, &currentCursorRow);
        // }

        // vga_set_cursor_position(currentCursorCol, currentCursorRow);
    } else if (keyCode == KEY_RIGHT) {
        // Arrow right pressed
        // int currentCursorCol = 0;
        // int currentCursorRow = 0;

        // vga_get_cursor_position(&currentCursorCol, &currentCursorRow);

        // if (savedCursorColPosition == -1 && savedCursorRowPosition == -1) {
        //     savedCursorColPosition = currentCursorCol;
        //     savedCursorRowPosition = currentCursorRow;
        // }

        // if (currentCursorCol < vga_get_max_width()) {
        //     currentCursorCol++;
        // } else {
        //     if (currentCursorRow < vga_get_max_height()) {
        //         currentCursorCol = 0;
        //         currentCursorRow++;
        //     } else {
        //         currentCursorCol = 0;
        //         vga_scroll_down(false);
        //         vga_get_cursor_position(&currentCursorCol, &currentCursorRow);
        //     }
        // }

        // vga_set_cursor_position(currentCursorCol, currentCursorRow);
    } else if (keyCode == KEY_PAGEUP) {
        vga_scroll_up();
    } else if (keyCode == KEY_PAGEDOWN) {
        vga_scroll_down(false);
    }
}

void vga_test() {
    // int cursorRow = 0;
    // int cursorCol = 0;

    // vga_get_cursor_position(&cursorCol, &cursorRow);
    // bool scrollGreatherThanVgaHeight = (linesCleared > 0 && ((vgaTextBufferVerticalScrollMax - linesCleared) - (vgaTextBufferVerticalScrollPos - linesCleared)) >= (HEIGHT - 1));

    // // kprintf("lines cleared %i\n", linesCleared);
    // kprintf("vS: %i - vSM: %i - cCol: %i - cRow: %i - bCol: %i - bRow: %i - lc: %i - sg: %i\n", vgaTextBufferVerticalScrollPos, vgaTextBufferVerticalScrollMax, cursorCol, cursorRow, vgaTextBufferCol, vgaTextBufferRow, linesCleared, scrollGreatherThanVgaHeight);
    // // kprintf("startRow: %i - endRow: %i - endContentRow: %i", vgaTextBufferStartRow, vgaTextBufferEndRow, vgaTextBufferEndContentRow);
    // // kprintf("bufferSize: %i - bufferContentRow: %i - bufferContentCol: %i - endRow: %i - bufferOffset: %i\n", VGA_TEXT_BUFFER_SIZE, vgaTextBufferEndContentRow, vgaTextBufferEndContentCol, vgaTextBufferEndRow, vgaTextBufferEndContentRow * WIDTH);
}