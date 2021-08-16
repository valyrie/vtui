// vtui common typedefs

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

#ifndef vtui_types_h
#define vtui_types_h

#include "vtui_macros.h"

// ## VTUI AXIS TYPES ##

#ifndef VTUI_AXIS

#define VTUI_AXIS unsigned int

#endif

#ifndef VTUI_LI_AXIS

#define VTUI_LI_AXIS long signed int

#endif

// ## VTUI COMMON TYPEDEFS ##

// TODO support non-truecolor text coloration (4bit, 6x6x6, greyscale)

// a vtui_color encodes a 32 bit RGB truecolor.
typedef struct vtui_color {
    union {
        struct {
            VTUI_BYTE red;
            VTUI_BYTE green;
            VTUI_BYTE blue;
            VTUI_BYTE _unused;
        };
        VTUI_BYTE bytes[4];
        VTUI_UINT32 color;
    };
} vtui_color;

// TODO formatting

// a vtui_cell is the smallest component of rendering, roughly corresponding to
// a single cell of the attached terminal.
typedef struct vtui_cell {
    char *grapheme;
    vtui_color fg;
    vtui_color bg;
} vtui_cell;

#endif