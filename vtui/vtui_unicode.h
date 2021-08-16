// vtui unicode functions

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

#ifndef vtui_unicode_h
#define vtui_unicode_h

#include "vtui_macros.h"
#include "vtui_types.h"

// ## VTUI UTF8 MACROS ##

#define _vtui_utf8_h1_mask  0b01111111
#define _vtui_utf8_h1_set   0
#define _vtui_utf8_h1_max   0x7F

#define _vtui_utf8_h2_mask  0b00011111
#define _vtui_utf8_h2_set   0b11000000
#define _vtui_utf8_h2_max   0x7FF

#define _vtui_utf8_h3_mask  0b00001111
#define _vtui_utf8_h3_set   0b11100000
#define _vtui_utf8_h3_max   0xFFFF

#define _vtui_utf8_h4_mask  0b00000111
#define _vtui_utf8_h4_set   0b11110000
#define _vtui_utf8_h4_max   0x10FFFF

#define _vtui_utf8_t_mask   0b00111111
#define _vtui_utf8_t_set    0b10000000
#define _vtui_utf8_t_bits   6

// ## VTUI UNICODE DECODING ##

// vtui uses utf8 internally, so only utf8 functions are provided

// decode a codepoint from a utf8 bytestream -- returns the number of bytes
// decoded via out parameter, return the codepoint directly. If a valid
// codepoint cannot be decoded, the returned character is undefined, and the
// number of bytes decoded will be set to zero.
VTUI_UINT32 _vtui_decode_utf8(const char *bytes, size_t max_bytes, int *decoded) {
    if (max_bytes >= 1) {
        // there are bytes
        char lead = bytes[0];
        if (vtui_within(lead, 0x00, 0x7F)) {
            // single byte
            *decoded = 1;
            return lead & _vtui_utf8_h1_mask;
        } else if (vtui_within(lead, 0xC2, 0xDF) && max_bytes >= 2) {
            // two bytes
            char tail1 = bytes[1];
            if (vtui_within(tail1, 0x80, 0xBF)) {
                // valid traling byte
                *decoded = 2;
                VTUI_INT32 codepoint = lead & _vtui_utf8_h2_mask;
                codepoint <<= _vtui_utf8_t_bits;
                codepoint |= tail1 & _vtui_utf8_t_mask; 
                return codepoint;
            } else {
                // invalid trailing byte
                *decoded = 0;
                return 0xFFFD;
            }
        } else if (vtui_within(lead, 0xE0, 0xEF) && max_bytes >= 3) {
            // three bytes
            // (not strictly well formed, some corner cases are not checked)
            char tail1 = bytes[1];
            char tail2 = bytes[2];
            if (vtui_within(tail1, 0x80, 0xBF) \
                && vtui_within(tail2, 0x80, 0xBF)) {
                *decoded = 3;
                VTUI_INT32 codepoint = lead & _vtui_utf8_h3_mask;
                codepoint <<= _vtui_utf8_t_bits;
                codepoint |= tail1 & _vtui_utf8_t_mask;
                codepoint <<= _vtui_utf8_t_bits;
                codepoint |= tail2 & _vtui_utf8_t_mask; 
                return codepoint;
            } else {
                // invalid trailing byte
                *decoded = 0;
                return 0xFFFD;
            }
        } else if (vtui_within(lead, 0xF0, 0xF4) && max_bytes >= 4) {
            // four bytes
            // (not strictly well formed, some corner cases are not checked)
            char tail1 = bytes[1];
            char tail2 = bytes[2];
            char tail3 = bytes[3];
            if (vtui_within(tail1, 0x80, 0xBF) \
                && vtui_within(tail2, 0x80, 0xBF) \
                && vtui_within(tail3, 0x80, 0xBF)) {
                *decoded = 3;
                VTUI_INT32 codepoint = lead & _vtui_utf8_h4_mask;
                codepoint <<= _vtui_utf8_t_bits;
                codepoint |= tail1 & _vtui_utf8_t_mask;
                codepoint <<= _vtui_utf8_t_bits;
                codepoint |= tail2 & _vtui_utf8_t_mask;
                codepoint <<= _vtui_utf8_t_bits;
                codepoint |= tail3 & _vtui_utf8_t_mask; 
                return codepoint;
            } else {
                // invalid trailing byte
                *decoded = 0;
                return 0xFFFD;
            }
        } else {
            // invalid leading byte
            *decoded = 0;
            return 0xFFFD;  // invalid character special
        }
    } else {
        // there are no bytes
        *decoded = 0;
        return 0x0;         // null character
    }
}

// ## VTUI UNICODE GRAPHEME SEGMENTATION ##

// 
int _vtui_utf8_gc_bytes(const char *bytes, size_t max_bytes) {
    //TODO
}

// TODO

#endif