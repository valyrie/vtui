// vtui -- header-only TUI library, implemented using virtual terminal codes.

/*
The Clear BSD License

Copyright (c) 2021 Valyrie Autumn
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted (subject to the limitations in the disclaimer
below) provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

     * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

     * Neither the name of the copyright holder nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef vtui_h
#define vtui_h

// VTUI SYMBOL NAMESPACING POLICY:
//  * vtui reserves the prefix vtui_ for its own public symbols, macros,
//    types, and other identifiers exported by it. public symbols are largely
//    stable between versions of this file, except for breaking changes.
//  * vtui may also use VTUI_ for public macros
//  * vtui reserves the prefix _vtui_ for its own private symbols, macros,
//    types, and other identifiers found in this file. private symbols can and
//    will change without warning between different versions.

// TODO explain vtui build control macros

// please note: all text that vtui works with is utf8 encoded, if you system
//              does not support utf8, you'll need to write a utf8->encoding 
//              function and hook it into whatever you provide as vtui_write.

// written in loose compliance with C89/C99

#include "vtui_macros.h"
#include "vtui_types.h"

// ## VTUI AXIS TYPES ##

#ifndef VTUI_AXIS_T

#define VTUI_AXIS_T unsigned int

#endif

#ifndef VTUI_LI_AXIS_T

#define VTUI_LI_AXIS_T long signed int

#endif

// ## VTUI CHARACTER CODES ##

#ifndef VTUI_NUL
#define VTUI_NUL "\0"
#endif

#ifndef VTUI_ESC
#define VTUI_ESC "\033"
#endif

#ifndef VTUI_BEL
#define VTUI_BEL "\a"
#endif

#ifndef VTUI_NL
#define VTUI_NL "\n"
#endif

#ifndef VTUI_CR
#define VTUI_CR "\r"
#endif

#define VTUI_CSI VTUI_ESC "["
#define VTUI_OSC VTUI_ESC "]"

// ## CURSOR POSITIONS ##

// when the location of the cursor on the screen isn't known (or is undefenied)
// it is said to be "in hyperspace". individual axes of the cursor can be in
// hyperspace independent of the other. valid cursor positions are 1 indexed --
// that is, the start at 1. 0 is used to represent that axis as being in
// hyperspace.

// For the curious, this is a nomenclature derived from comments found in the
// source code to ncurses/ncursesw.

#define VTUI_HYPERSPACE 0

// a home is considered to be the first cell of a given axis, and the top-left
// corner of the screen when speaking in terms of both axes

#define VTUI_HOME 1

// ## VTUI FUNCTION TYPEDEFS ##

// memory manipulation function typedefs

typedef void *(vtui_malloc)(VTUI_SIZE_T);                            //malloc
typedef void (vtui_free)(void *);                                    //free
typedef void (vtui_memcpy)(void *, const void *, VTUI_SIZE_T);       //memcpy

// the library provided behavior of realloc differs between C89 and C99 vtui
// expects a realloc that behaves according to C99 -- returning NULL always
// indicates total failure of realloc, while in C89 NULL could be returned on
// successful reallocation to a size of zero (free, basically); additionally,
// to avoid any confusion as to the success of realloc with a C89 library,
// this function will never be invoked with a size_t equal to zero.
typedef void *(vtui_realloc)(void *, VTUI_SIZE_T);                   //realloc

// i/o function typedefs

// write out bytes to a vtui console, returns bytes written
//  special care should be taken when implementing vtui_write to ensure that it
//  writes bytes as atomically as possible, otherwise the terminal may flicker
//  agressively,  due to the inconsistent state non-atomic writes will leave it
//  in. expects that the number of bytes written will equal the number of bytes
//  provided, otherwise vtui will treat it as an error (incomplete write)
typedef int (vtui_write)(void *file, const char *buf, VTUI_SIZE_T bytes);

// string formatting function typedefs

// a restricted version of snprintf, the only format specifiers used are
// * %u (unsigned decimal integer) 
// * %s.* (string, variable maximum number of characters)
// * %lc (long character / wint or smthn, used to print a unicode codepoint)
// returns bytes written, or a negative number if an error occurs; string is 
// only assumed to have been successfully written when the return value is 
// greater than zero, but less than bytes
typedef int (vtui_snprintf)(char *buf, size_t bytes, const char *format, ...);

// ## quick vtui helper functions ##

size_t _vtui_strnt_bytes(const char *str) {
    size_t i = 0;
    while (str[i] != VTUI_NUL[0]) {
        i++;
    }
    return i;
}

// ## VTUI MAJOR TYPEDEFS ##

// TODO ui elements
//  box (base)
//  textbox
//  contentbox
//  framedbox
//  list
//  table
//  canvas

// a vtui_cursor_state encodes the state of the cursor
typedef struct vtui_cursor_state {
        VTUI_AXIS_T row;   // rows start at 1 -- 0 means hyperspace
        VTUI_AXIS_T col;   // cols start at 1 -- 0 means hyperspace
        VTUI_BOOL_T invisible;
} vtui_cursor_state_t;

// private update buffer state enum -- used to fuse commands where possible
typedef enum _vtui_update_buffer_state {
    _vtui_update_buffer_state_last_cmd_was_not_sgr = 0,
    _vtui_update_buffer_state_last_cmd_was_sgr
} _vtui_update_buffer_state_t;

// private updated buffer structure -- stores chars while building an update
typedef struct _vtui_update_buffer {
    char *text;
    size_t cur_bytes;
    size_t capacity;
    _vtui_update_buffer_state_t state;
} _vtui_update_buffer_t;

// primary vtui structure -- represents a single vtui on a single output
typedef struct vtui {
    // first up, fn pntrs

    vtui_malloc *malloc;
    vtui_free *free;
    vtui_memcpy *memcpy;
    vtui_realloc *realloc;
    vtui_write *write;
    vtui_snprintf *snprintf;

    // variables

    void *out_file; //may not actually be a voidp: using it as a catch-all type
    
    // TODO rendering variables
    // boxes, front boxes

    vtui_cursor_state_t cursor;
    VTUI_BOOL_T beep;

    // "front" variables, used to track the state the output was left in
    vtui_cursor_state_t front_cursor;
    vtui_color_t front_fg;
    vtui_color_t front_bg;
    VTUI_BOOL_T front_colors_initalized;

    // internal update buffer -- used to build updates as one long string
    _vtui_update_buffer_t _buffer;
} vtui_t;

// ## VTUI UPDATE BUFFER ROUTINES ##

// push bytes into a vtui's update buffer
//  internally handles memory reallocation, and can fail with VTUI_ENOMEM; also
//  supports overwriting some of the bytes already in the buffer, but does not
//  sanity check overwrite to prevent underflows, or that bytes is greater than
//  overwrite (unsigned math goes out the window when you subtract from 0)
int _vtui_pushBytes(vtui_t *vtui, const char *buf, size_t bytes, size_t over) {
    if (vtui->_buffer.cur_bytes + bytes - over > vtui->_buffer.capacity) {
        size_t newcapacity = vtui->_buffer.capacity;
        while (vtui->_buffer.cur_bytes + bytes - over > newcapacity) {
            newcapacity <<= 1;
        }
        void *pntr = vtui->realloc(vtui->_buffer.text, newcapacity);
        if (pntr != VTUI_NULL) {
            vtui->_buffer.text = (char *) pntr;
            vtui->_buffer.capacity = newcapacity;
        } else {
            // give up if null pntr
            return VTUI_ENOMEM;
        }
    }
    // memcpy bytes into the buffer
    size_t offset = vtui->_buffer.cur_bytes - over;
    vtui->memcpy(&vtui->_buffer.text[offset], buf, bytes);
    vtui->_buffer.cur_bytes += bytes - over;
    // success!
    return VTUI_OK;
}

// flush a vtui's update buffer to the screen
//  can fail with VTUI_EIO (generic I/O error); if an error occurs while
//  writing, a partial write may have occurred, leaving the vtui in an
//  inconsisent state!
int _vtui_flush(vtui_t *vtui) {
    if (vtui->_buffer.cur_bytes > 0){
        _vtui_update_buffer_t *buffer = &vtui->_buffer;
        int written = vtui->write(
            vtui->out_file, buffer->text, buffer->cur_bytes);
        if (written == buffer->cur_bytes) {
            buffer->cur_bytes = 0;
            return VTUI_OK;
        } else {
            // incomplete write, or some other error!
            return VTUI_EIO;
        }
    } else {
        // no bytes to write, silent success
        return VTUI_OK;
    }
}

// push a non-sgr command
int _vtui_pushCmd(vtui_t *vtui, const char *cmd, int bytes) {
    int err = _vtui_pushBytes(vtui, cmd, bytes, 0);
    if (err == VTUI_OK) {
        vtui->_buffer.state = _vtui_update_buffer_state_last_cmd_was_not_sgr;
        return VTUI_OK;
    } else {
        return err;
    }
}

// push a sgr command
int _vtui_pushSgrCmd(vtui_t *vtui, const char *cmd, int bytes) {
    int overwrite;
    char *base;
    size_t len;
    if (vtui->_buffer.state == _vtui_update_buffer_state_last_cmd_was_sgr) {
        overwrite = _vtui_strnt_bytes("m"); // splat over the m of the prev cmd
        len  = bytes - _vtui_strnt_bytes(VTUI_CSI); // skip the csi
        char *base = ((char *) cmd) + _vtui_strnt_bytes(VTUI_CSI); // ditto
    } else {
        overwrite = 0;
        base = ((char *) cmd);
        len = bytes;
    }
    int err = _vtui_pushBytes(vtui, base, len, overwrite);
    if (err == VTUI_OK) {
        vtui->_buffer.state = _vtui_update_buffer_state_last_cmd_was_not_sgr;
        return VTUI_OK;
    } else {
        return err;
    }
}

// ## VTUI VT COMMANDS ##

// reset the terminal's Select Graphics Rendition state
int _vtui_vt_resetSgr(vtui_t *vtui) {
    const char *cmd = VTUI_CSI "0m";
    // VTUI doesn't know how to handle non-truecolor colors
    vtui->front_colors_initalized = VTUI_FALSE;
    return _vtui_pushSgrCmd(vtui, cmd, _vtui_strnt_bytes(cmd));
    
}

// unhide the cursor, making it visible
int _vtui_vt_showCursor(vtui_t *vtui) {
    const char *cmd = VTUI_CSI "?25h";
    vtui->front_cursor.invisible = VTUI_FALSE;
    return _vtui_pushCmd(vtui, cmd, _vtui_strnt_bytes(cmd));
}

// hide the cursor, making it invisible
int _vtui_vt_hideCursor(vtui_t *vtui) {
    const char *cmd = VTUI_CSI "?25l";
    vtui->front_cursor.invisible = VTUI_TRUE;
    return _vtui_pushCmd(vtui, cmd, _vtui_strnt_bytes(cmd));
}

// make a beep on the terminal
int _vtui_vt_beep(vtui_t *vtui) {
    const char *cmd = VTUI_BEL;
    vtui->beep = VTUI_FALSE; //do not beep again
    return _vtui_pushCmd(vtui, cmd, _vtui_strnt_bytes(cmd));
}

// clears the screen and sometimes resets cursor position to the top left,
// colors need to be initalized before calling, otherwise will fail with
// VTUI_EWRONGSTATE.
int _vtui_vt_clearScreen(vtui_t *vtui) {
    const char *cmd = VTUI_CSI "2J";
    if (vtui->front_colors_initalized) {
        // no idea where this puts the cursor -- send it off to hyperspace
        vtui->front_cursor.row = VTUI_HYPERSPACE;
        vtui->front_cursor.col = VTUI_HYPERSPACE;
        //TODO clear the front surface!
        return _vtui_pushCmd(vtui, cmd, _vtui_strnt_bytes(cmd));
    } else {
        return VTUI_EWRONGSTATE;
    }
}

// enter the alternate screen buffer
int _vtui_vt_enterAlt(vtui_t *vtui) {
    const char *cmd = VTUI_CSI "1049h";
    // no idea where this puts the cursor -- send it off to hyperspace
    vtui->front_cursor.row = VTUI_HYPERSPACE;
    vtui->front_cursor.col = VTUI_HYPERSPACE;
    return _vtui_pushCmd(vtui, cmd, _vtui_strnt_bytes(cmd));
    //TODO if the alternate screen buffer is not supported, clear sufficient
    // space to avoid overwriting scrollback buffer text
}

// exit the alternate screen buffer
int _vtui_vt_exitAlt(vtui_t *vtui) {
    const char *cmd = VTUI_CSI "1049l";
    // no idea where this puts the cursor -- send it off to hyperspace
    vtui->front_cursor.row = VTUI_HYPERSPACE;
    vtui->front_cursor.col = VTUI_HYPERSPACE;
    return _vtui_pushCmd(vtui, cmd, _vtui_strnt_bytes(cmd));
}

// cursor-movement terminology:
// * home (1)   / the first position in a row / column
// * home (2)   / home can also mean row/col 0/0 -- the cursor's home position 
// * hyperspace / where the cursor lives when vtui doesn't know where it is

// note that moveTo utilizes nondestructive NLs / CRs and a mess of CSI codes

// move the cursor from its current position to the specified position, by any
// means necessary and using the minimum amount of bytes
int _vtui_vt_moveTo(vtui_t *vtui, int row, int col) {
    // if the cursor is not already in position
    if (vtui->front_cursor.row != row && vtui->front_cursor.col != col) {
        // check for hyperspace
        VTUI_BOOL_T srcrow_hyperspace = row == VTUI_HYPERSPACE;
        VTUI_BOOL_T srccol_hyperspace = col == VTUI_HYPERSPACE;
        VTUI_BOOL_T any_hyperspace = srcrow_hyperspace | srccol_hyperspace;

        // check if destination is a home
        VTUI_BOOL_T dstrow_home = row == VTUI_HOME;
        VTUI_BOOL_T dstcol_home = col == VTUI_HOME;
        VTUI_BOOL_T screen_home = dstrow_home & dstcol_home; 

        // compute signed distance
        VTUI_LI_AXIS_T row_dist = (VTUI_LI_AXIS_T) row \
            - (VTUI_LI_AXIS_T) vtui->front_cursor.row;
        VTUI_LI_AXIS_T col_dist = (VTUI_LI_AXIS_T) col \
            - (VTUI_LI_AXIS_T) vtui->front_cursor.col;

        // compute signed distance from home
        VTUI_LI_AXIS_T hrow_dist = (VTUI_LI_AXIS_T) row - VTUI_HOME;
        VTUI_LI_AXIS_T hcol_dist = (VTUI_LI_AXIS_T) col - VTUI_HOME;

        // commands
        const char *relup_fmt = VTUI_CSI "%uA";
        const char *reldown_fmt = VTUI_CSI "%uB";
        const char *relfore_fmt = VTUI_CSI "%uC";
        const char *relback_fmt = VTUI_CSI "%uD";
        const char *nextline_fmt = VTUI_CSI "%uE";
        const char *prevline_fmt = VTUI_CSI "%uF";
        const char *abscol_fmt = VTUI_CSI "%uG";
        const char *absrc_fmt = VTUI_CSI "%u;%uH";

        int bytes;
        char cmd_buf[32]; // oversized, to be safe

        // hopefully the cursor isn't in hyperspace
        if (!any_hyperspace) {
            if (row_dist == 0) {
                // handle same-row movement
                if (vtui_abs(col_dist) < hcol_dist) {
                    // if relative movement is closer
                    if (col_dist >= 0) {
                        // positive column movement
                        bytes = vtui->snprintf(cmd_buf, 32,
                            relfore_fmt, col_dist);
                    } else {
                        // negative column movement
                        bytes = vtui->snprintf(cmd_buf, 32,
                            relback_fmt, -col_dist);
                    }
                } else {
                    // if absolute movement is closer
                    bytes = vtui->snprintf(cmd_buf, 32, abscol_fmt, col);
                }
            } else if (col_dist == 0 && vtui_abs(row_dist) < hrow_dist) {
                // do relative same-col movement if possible
                if (row_dist >= 0) {
                    // positive row movement
                    bytes = vtui->snprintf(cmd_buf, 32,
                        reldown_fmt, row_dist);
                } else {
                    // negative column movement
                    bytes = vtui->snprintf(cmd_buf, 32,
                        relup_fmt, -row_dist);
                }
            } else {
                // cursor has moved in two axes, handle it

                // TODO relative dual-axis movement
                // TODO mixed relative / absolute movement

                if (dstcol_home && vtui_abs(row_dist) < hrow_dist) {
                    // shift to the start of a relative line
                    if (row_dist >= 0) {
                        // positive row movement
                        bytes = vtui->snprintf(cmd_buf, 32,
                            nextline_fmt, row_dist);
                    } else {
                        // negative column movement
                        bytes = vtui->snprintf(cmd_buf, 32,
                            prevline_fmt, -row_dist);
                    }
                } else {
                    // full absolute movement is closer
                    bytes = vtui->snprintf(cmd_buf, 32, absrc_fmt, row, col);
                }
            }
        } else {
            // hyperspace encountered, leave using absolute movement
            if (row_dist == 0 && !srcrow_hyperspace) {
                // if possible, use column absolute movement
                bytes = vtui->snprintf(cmd_buf, 32, abscol_fmt, col);
            } else {
                // fall back to full absolute movement
                bytes = vtui->snprintf(cmd_buf, 32, absrc_fmt, row, col);
            }
        }
        if (bytes > 0 && bytes < 32) {
            // string successfully written
            vtui->front_cursor.row = row;
            vtui->front_cursor.col = col;
            return _vtui_pushSgrCmd(vtui, cmd_buf, bytes);
        } else {
            // failed to write formatted string
            return VTUI_EFMT;
        }
    } else {
        // no change
        return VTUI_OK;
    }
}

// set the foreground color to the specified 24-bit truecolor color
int _vtui_vt_setFg(vtui_t *vtui, VTUI_BYTE_T r, VTUI_BYTE_T g, VTUI_BYTE_T b) {
    const char *cmd_fmt = VTUI_CSI "38;2;%u;%u;%um";
    char cmd_buf[32]; // oversized, to be safe
    int bytes = vtui->snprintf(cmd_buf, 32, cmd_fmt, r, g, b);
    if (bytes > 0 && bytes < 32) {
        // string successfully written
        return _vtui_pushSgrCmd(vtui, cmd_buf, bytes);
    } else {
        // failed to write formatted string
        return VTUI_EFMT;
    }
}

// set the background color to the specified 24-bit truecolor color
int _vtui_vt_setBg(vtui_t *vtui, VTUI_BYTE_T r, VTUI_BYTE_T g, VTUI_BYTE_T b) {
    const char *cmd_fmt = VTUI_CSI "38;2;%u;%u;%um";
    char cmd_buf[32]; // oversized, to be safe
    int bytes = vtui->snprintf(cmd_buf, 32, cmd_fmt, r, g, b);
    if (bytes > 0 && bytes < 32) {
        // string successfully written
        return _vtui_pushSgrCmd(vtui, cmd_buf, bytes);
    } else {
        // failed to write formatted string
        return VTUI_EFMT;
    }
}

// push a single unicode codepoint to an update buffer
int _vtui_vt_pushCodepoint(vtui_t *vtui, VTUI_UINT32_T character) {
    const char *cmd_fmt = "%lc";
    char cmd_buf[32]; // way way way oversized, to be safe
    int bytes = vtui->snprintf(cmd_buf, 32, cmd_fmt, character);
    if (bytes > 0 && bytes < 32) {
        // string successfully written
        // TODO update cursor position!
        return _vtui_pushSgrCmd(vtui, cmd_buf, bytes);
    } else {
        // failed to write formatted string
        return VTUI_EFMT;
    }
}

//TODO commands:
// clear large chunks of the screen (instead of splatting a bunch of spaces)
// non truecolor colors!
// all kinds of sgr formatting stuff???
// OSC set title
// OSC hyperlink
// full terminal reset
// enable/disable bracketed paste
// change input mode
// tabulation stops???
// ???

// ## VTUI RENDERING ROUTINES ##

//
int vtui_update(vtui *vtui, VTUI_BOOL_T full_redraw) {

}

//TODO

//TODO

#endif